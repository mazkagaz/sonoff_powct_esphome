#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "cse7761_core_conversion.h"
#include "cse7761_core_stock_pipeline.h"

#ifndef GOLDEN_MEASUREMENTS_FILE
#error "GOLDEN_MEASUREMENTS_FILE must be defined"
#endif

using namespace cse7761_core;

namespace {

std::string trim(const std::string &value) {
  const size_t first = value.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  const size_t last = value.find_last_not_of(" \t\r\n");
  return value.substr(first, last - first + 1);
}

std::vector<std::string> split_csv(const std::string &line) {
  std::vector<std::string> fields;
  std::string field;
  bool quoted = false;
  for (char character : line) {
    if (character == '"') {
      quoted = !quoted;
    } else if (character == ',' && !quoted) {
      fields.push_back(trim(field));
      field.clear();
    } else {
      field.push_back(character);
    }
  }
  fields.push_back(trim(field));
  return fields;
}

std::unordered_map<std::string, size_t> header_index(const std::string &header) {
  std::unordered_map<std::string, size_t> index;
  const auto fields = split_csv(header);
  for (size_t i = 0; i < fields.size(); i++) {
    index[fields[i]] = i;
  }
  return index;
}

const std::string &field(const std::vector<std::string> &row,
                         const std::unordered_map<std::string, size_t> &index,
                         const std::string &name) {
  const auto it = index.find(name);
  if (it == index.end() || it->second >= row.size()) {
    FAIL("Missing CSV column: " << name);
  }
  return row[it->second];
}

long long signed_number(const std::vector<std::string> &row,
                        const std::unordered_map<std::string, size_t> &index,
                        const std::string &name) {
  return std::strtoll(field(row, index, name).c_str(), nullptr, 10);
}

double double_number(const std::vector<std::string> &row,
                     const std::unordered_map<std::string, size_t> &index,
                     const std::string &name) {
  return std::strtod(field(row, index, name).c_str(), nullptr);
}

double drift_pct(long long baseline, long long value) {
  const double denominator = std::max(std::abs(static_cast<double>(baseline)), 1.0);
  return std::abs(static_cast<double>(value - baseline)) / denominator * 100.0;
}

}  // namespace

TEST_CASE("stock pipeline drift against approved production baseline") {
  std::ifstream measurements_file(GOLDEN_MEASUREMENTS_FILE);
  REQUIRE_MESSAGE(measurements_file.good(), "Cannot open " << GOLDEN_MEASUREMENTS_FILE);
  std::string line;
  REQUIRE(std::getline(measurements_file, line));
  const auto header = header_index(line);
  const auto identity = identity_stock_calibration();

  size_t rows = 0;
  while (std::getline(measurements_file, line)) {
    if (trim(line).empty()) {
      continue;
    }
    const auto row = split_csv(line);
    const std::string case_id = field(row, header, "case_id");
    const std::string source = field(row, header, "source");
    const std::string captured_at = field(row, header, "captured_at");
    const std::string approved_by = field(row, header, "approved_by");
    const std::string approval_reason = field(row, header, "approval_reason");
    const bool valid_source = source == "baseline" || source == "prod";
    CHECK_MESSAGE(valid_source,
                  "Golden case " << case_id << " has invalid source: " << source);
    CHECK_MESSAGE(!captured_at.empty(), "Golden case " << case_id << " has no captured_at");
    CHECK_MESSAGE(!approved_by.empty(), "Golden case " << case_id << " has no approved_by");
    CHECK_MESSAGE(!approval_reason.empty(),
                  "Golden case " << case_id << " has no approval_reason");
    if (source == "prod") {
      CHECK_MESSAGE(approved_by != "baseline",
                    "Production golden case " << case_id << " still has baseline approval");
    }

    const uint32_t raw_voltage = signed_number(row, header, "raw_voltage");
    const uint32_t raw_current = signed_number(row, header, "raw_current");
    const uint32_t raw_power = signed_number(row, header, "raw_power");
    const uint32_t coefficient_voltage = signed_number(row, header, "coeff_voltage");
    const uint32_t coefficient_current = signed_number(row, header, "coeff_current");
    const uint32_t coefficient_power = signed_number(row, header, "coeff_power");

    uint32_t coefficients[8] = {};
    coefficients[RMS_UC] = coefficient_voltage;
    coefficients[RMS_IAC] = coefficient_current;
    coefficients[POWER_PAC] = coefficient_power;

    const float production_voltage =
        voltage_from_raw(raw_voltage, coefficient_for_unit(RMS_UC, coefficients));
    const float production_current =
        current_from_raw(raw_current, coefficient_for_unit(RMS_IAC, coefficients));
    const float production_power =
        power_from_raw(raw_power, coefficient_for_unit(POWER_PAC, coefficients));
    const long long production_voltage_uv = std::llround(production_voltage * 1e6);
    const long long production_current_ua = std::llround(production_current * 1e6);
    const long long production_power_uw = std::llround(production_power * 1e6);

    const StockPipelineInput stock_input{raw_voltage, raw_current, raw_power,
                                         coefficient_voltage, coefficient_current,
                                         coefficient_power, 0};
    const auto stock = run_stock_pipeline(stock_input, identity);
    const long long stock_voltage_uv = static_cast<long long>(stock.voltage_mv) * 1000;
    const long long stock_current_ua = static_cast<long long>(stock.current_ma) * 1000;
    const long long stock_power_uw =
        static_cast<long long>(stock.power_mw) * 1000 * stock.power_sign;

    CHECK_MESSAGE(production_voltage_uv ==
                      signed_number(row, header, "production_voltage_uv"),
                  case_id << ": production voltage mismatch");
    CHECK_MESSAGE(production_current_ua ==
                      signed_number(row, header, "production_current_ua"),
                  case_id << ": production current mismatch");
    CHECK_MESSAGE(production_power_uw ==
                      signed_number(row, header, "production_power_uw"),
                  case_id << ": production power mismatch");
    CHECK_MESSAGE(stock_voltage_uv == signed_number(row, header, "stock_voltage_uv"),
                  case_id << ": stock voltage mismatch");
    CHECK_MESSAGE(stock_current_ua == signed_number(row, header, "stock_current_ua"),
                  case_id << ": stock current mismatch");
    CHECK_MESSAGE(stock_power_uw == signed_number(row, header, "stock_power_uw"),
                  case_id << ": stock power mismatch");

    const double voltage_drift = drift_pct(production_voltage_uv, stock_voltage_uv);
    const double current_drift = drift_pct(production_current_ua, stock_current_ua);
    const double power_drift = drift_pct(production_power_uw, stock_power_uw);
    const double max_drift = std::max({voltage_drift, current_drift, power_drift});
    const double threshold = double_number(row, header, "drift_threshold_pct");
    const std::string drift_approved_by = field(row, header, "drift_approved_by");
    const std::string drift_reason = field(row, header, "drift_approval_reason");
    const double approved_drift_pct = double_number(row, header, "drift_approved_pct");

    CHECK_MESSAGE(std::abs(voltage_drift - double_number(row, header, "voltage_drift_pct")) <
                      0.01,
                  case_id << ": voltage drift changed");
    CHECK_MESSAGE(std::abs(current_drift - double_number(row, header, "current_drift_pct")) <
                      0.01,
                  case_id << ": current drift changed");
    CHECK_MESSAGE(std::abs(power_drift - double_number(row, header, "power_drift_pct")) <
                      0.01,
                  case_id << ": power drift changed");

    if (max_drift > threshold + 0.01) {
      CHECK_MESSAGE(!drift_approved_by.empty(),
                    case_id << ": drift " << max_drift << "% exceeds " << threshold
                            << "% without drift_approved_by");
      CHECK_MESSAGE(!drift_reason.empty(),
                    case_id << ": drift " << max_drift << "% exceeds " << threshold
                            << "% without drift_approval_reason");
      CHECK_MESSAGE(max_drift <= approved_drift_pct + 0.01,
                    case_id << ": drift " << max_drift << "% exceeds approved "
                            << approved_drift_pct << "%");
    }
    rows++;
  }

  CHECK_MESSAGE(rows > 0, "No golden measurement rows were loaded");
}
