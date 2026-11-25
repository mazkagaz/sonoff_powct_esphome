#include <doctest/doctest.h>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "cse7761_core_conversion.h"

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

uint32_t unsigned_number(const std::vector<std::string> &row,
                         const std::unordered_map<std::string, size_t> &index,
                         const std::string &name) {
  return static_cast<uint32_t>(std::strtoul(field(row, index, name).c_str(), nullptr, 10));
}

long long signed_number(const std::vector<std::string> &row,
                        const std::unordered_map<std::string, size_t> &index,
                        const std::string &name) {
  return std::strtoll(field(row, index, name).c_str(), nullptr, 10);
}

}  // namespace

TEST_CASE("production golden measurement pipeline matches approved baseline") {
  std::ifstream measurements_file(GOLDEN_MEASUREMENTS_FILE);
  REQUIRE_MESSAGE(measurements_file.good(), "Cannot open " << GOLDEN_MEASUREMENTS_FILE);
  std::string line;
  REQUIRE(std::getline(measurements_file, line));
  const auto header = header_index(line);

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

    const uint32_t raw_voltage = unsigned_number(row, header, "raw_voltage");
    const uint32_t raw_current = unsigned_number(row, header, "raw_current");
    const uint32_t raw_power = unsigned_number(row, header, "raw_power");
    const uint32_t coefficient_voltage = unsigned_number(row, header, "coeff_voltage");
    const uint32_t coefficient_current = unsigned_number(row, header, "coeff_current");
    const uint32_t coefficient_power = unsigned_number(row, header, "coeff_power");

    uint32_t coefficients[8] = {};
    coefficients[RMS_UC] = coefficient_voltage;
    coefficients[RMS_IAC] = coefficient_current;
    coefficients[POWER_PAC] = coefficient_power;

    const float voltage =
        voltage_from_raw(raw_voltage, coefficient_for_unit(RMS_UC, coefficients));
    const float current =
        current_from_raw(raw_current, coefficient_for_unit(RMS_IAC, coefficients));
    const float power =
        power_from_raw(raw_power, coefficient_for_unit(POWER_PAC, coefficients));

    const long long voltage_uv = std::llround(voltage * 1e6);
    const long long current_ua = std::llround(current * 1e6);
    const long long power_uw = std::llround(power * 1e6);

    CHECK_MESSAGE(voltage_uv == signed_number(row, header, "expected_voltage_uv"),
                  case_id << ": expected voltage "
                          << field(row, header, "expected_voltage_uv") << " uV, got "
                          << voltage_uv);
    CHECK_MESSAGE(current_ua == signed_number(row, header, "expected_current_ua"),
                  case_id << ": expected current "
                          << field(row, header, "expected_current_ua") << " uA, got "
                          << current_ua);
    CHECK_MESSAGE(power_uw == signed_number(row, header, "expected_power_uw"),
                  case_id << ": expected power "
                          << field(row, header, "expected_power_uw") << " uW, got "
                          << power_uw);
    rows++;
  }

  CHECK_MESSAGE(rows > 0, "No golden measurement rows were loaded");
}
