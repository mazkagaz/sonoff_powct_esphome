#pragma once

#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace cse7761_core {

struct QuadraticCoefficients {
  double a;
  double b;
  double c;
};

struct StockCalibration {
  QuadraticCoefficients current[2];
  QuadraticCoefficients voltage[2];
  QuadraticCoefficients power[2];
  bool valid{false};
};

struct StockMeasurement {
  double voltage_mv;
  double current_ma;
  double power_mw;
};

inline QuadraticCoefficients identity_quadratic() { return {0.0, 1000000.0, 0.0}; }

inline StockCalibration identity_stock_calibration() {
  StockCalibration calibration{};
  for (uint8_t range = 0; range < 2; range++) {
    calibration.current[range] = identity_quadratic();
    calibration.voltage[range] = identity_quadratic();
    calibration.power[range] = identity_quadratic();
  }
  calibration.valid = true;
  return calibration;
}

inline double apply_quadratic(const QuadraticCoefficients &coefficients, double value) {
  const double result = (coefficients.a * value * value) / 1.0e7 +
                        (coefficients.b * value) / 1.0e6 +
                        coefficients.c / 1.0e6;
  const double truncated = std::trunc(result);
  return truncated < 0.0 ? 0.0 : truncated;
}

inline uint8_t select_stock_range(double current_ma, double threshold_ma = 500.0) {
  return current_ma >= threshold_ma ? 1 : 0;
}

inline StockMeasurement apply_stock_calibration(const StockCalibration &calibration,
                                                double voltage_mv, double current_ma,
                                                double power_mw, uint8_t range) {
  if (!calibration.valid || range > 1) {
    return {0.0, 0.0, 0.0};
  }
  return {
      apply_quadratic(calibration.voltage[range], voltage_mv),
      apply_quadratic(calibration.current[range], current_ma),
      apply_quadratic(calibration.power[range], power_mw),
  };
}

inline StockMeasurement apply_stock_calibration_ranges(
    const StockCalibration &calibration, double voltage_mv, double current_ma,
    double power_mw, uint8_t voltage_range, uint8_t current_power_range) {
  if (!calibration.valid || voltage_range > 1 || current_power_range > 1) {
    return {0.0, 0.0, 0.0};
  }
  return {
      apply_quadratic(calibration.voltage[voltage_range], voltage_mv),
      apply_quadratic(calibration.current[current_power_range], current_ma),
      apply_quadratic(calibration.power[current_power_range], power_mw),
  };
}

inline bool parse_numbers(const std::string &text, size_t begin, size_t end,
                          std::vector<double> &values) {
  size_t position = begin;
  while (position < end) {
    while (position < end &&
           (std::isspace(static_cast<unsigned char>(text[position])) ||
            text[position] == ',' || text[position] == '[' || text[position] == ']')) {
      position++;
    }
    if (position >= end) {
      break;
    }

    char *parsed_end = nullptr;
    const double value = std::strtod(text.c_str() + position, &parsed_end);
    if (parsed_end == text.c_str() + position) {
      return false;
    }
    values.push_back(value);
    position = static_cast<size_t>(parsed_end - text.c_str());
  }
  return true;
}

inline bool parse_stock_calibration(const std::string &text, StockCalibration &calibration) {
  std::vector<double> values;

  const size_t first_bracket = text.find('[');
  if (first_bracket != std::string::npos) {
    size_t position = first_bracket;
    bool found_closing = false;
    while (position != std::string::npos) {
      const size_t open = text.find('[', position);
      if (open == std::string::npos) {
        break;
      }
      const size_t close = text.find(']', open);
      if (close == std::string::npos) {
        return false;
      }
      if (!parse_numbers(text, open + 1, close, values)) {
        return false;
      }
      position = close + 1;
      found_closing = true;
    }
    if (!found_closing) {
      return false;
    }
  } else if (!parse_numbers(text, 0, text.size(), values)) {
    return false;
  }

  if (values.size() != 18) {
    return false;
  }

  calibration.current[0] = {values[0], values[1], values[2]};
  calibration.voltage[0] = {values[3], values[4], values[5]};
  calibration.power[0] = {values[6], values[7], values[8]};
  calibration.current[1] = {values[9], values[10], values[11]};
  calibration.voltage[1] = {values[12], values[13], values[14]};
  calibration.power[1] = {values[15], values[16], values[17]};
  calibration.valid = true;
  return true;
}

}  // namespace cse7761_core
