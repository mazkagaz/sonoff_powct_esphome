#pragma once

#include <cstdint>

namespace cse7761_core {

constexpr uint32_t kCalibrationMeasurements = 20;
constexpr double kCurrentCalibrationScale = 10.46;
constexpr double kPowerCalibrationScale = -8.89;

struct CalibrationOffsets {
  double current_a;
  double current_b;
  double power_a;
  double power_b;
};

inline CalibrationOffsets apply_calibration_offsets(CalibrationOffsets offsets,
                                                    double sum_current_b,
                                                    double sum_power_b,
                                                    uint32_t count) {
  if (count == 0) {
    return offsets;
  }

  offsets.current_b -= sum_current_b / static_cast<double>(count);
  offsets.power_b -= sum_power_b / static_cast<double>(count);
  offsets.current_a = kCurrentCalibrationScale * offsets.current_b;
  offsets.power_a = kPowerCalibrationScale * offsets.power_b;
  return offsets;
}

}  // namespace cse7761_core
