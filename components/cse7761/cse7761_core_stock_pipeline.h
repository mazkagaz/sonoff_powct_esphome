#pragma once

#include <cstdint>

#include "cse7761_core_stock_calibration.h"
#include "cse7761_core_stock_conversion.h"

namespace cse7761_core {

struct StockPipelineInput {
  uint32_t raw_voltage;
  uint32_t raw_current;
  uint32_t raw_power;
  uint32_t coefficient_voltage;
  uint32_t coefficient_current;
  uint32_t coefficient_power;
  uint8_t previous_range;
};

struct StockPipelineOutput {
  uint32_t voltage_mv;
  uint32_t current_ma;
  uint32_t power_mw;
  uint8_t range;
  int8_t power_sign;
};

inline StockPipelineOutput run_stock_pipeline(const StockPipelineInput &input,
                                              const StockCalibration &calibration) {
  const uint32_t voltage_mv = stock_voltage_mv(input.raw_voltage, input.coefficient_voltage);
  const uint32_t current_ma = stock_current_ma(input.raw_current, input.coefficient_current);
  const uint8_t range = select_stock_range(current_ma);
  const uint32_t power_mw = stock_power_mw(input.raw_power, input.coefficient_power);
  const StockMeasurement measurement = apply_stock_calibration_ranges(
      calibration, voltage_mv, current_ma, power_mw, input.previous_range, range);

  const int8_t power_sign = static_cast<int32_t>(input.raw_power) < 0 ? -1 : 1;
  return {
      static_cast<uint32_t>(measurement.voltage_mv),
      static_cast<uint32_t>(measurement.current_ma),
      static_cast<uint32_t>(measurement.power_mw),
      range,
      power_sign,
  };
}

}  // namespace cse7761_core
