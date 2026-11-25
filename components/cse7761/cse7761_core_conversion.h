#pragma once

#include <cstdint>

#include "cse7761_core_protocol.h"
#include "cse7761_core_registers.h"

namespace cse7761_core {

inline uint32_t coefficient_for_unit(Measure unit, const uint32_t coefficients[8]) {
  const uint32_t coefficient = coefficients[unit];
  if (coefficient == 0) {
    return 0;
  }

  switch (unit) {
    case RMS_IAC:
    case RMS_IBC:
      return (0x800000U * 100U / coefficient) * 10U / 4.7;
    case RMS_UC:
      return 0x400000U * 100U / coefficient;
    case POWER_PAC:
    case POWER_PBC:
      return 0x80000000U / coefficient / 4.7;
    case POWER_SC:
    case ENERGY_AC:
    case ENERGY_BC:
      return 0x80000000U / coefficient;
  }
  return 0;
}

inline float voltage_from_raw(uint32_t raw, uint32_t coefficient) {
  if (raw >= 0x800000U || coefficient == 0) {
    return 0.0f;
  }
  return static_cast<float>(raw) / static_cast<float>(coefficient);
}

inline float current_from_raw(uint32_t raw, uint32_t coefficient) {
  if (coefficient == 0) {
    return 0.0f;
  }
  return static_cast<float>(sign_extend_24(raw)) / static_cast<float>(coefficient);
}

inline float power_from_raw(uint32_t raw, uint32_t coefficient) {
  if (coefficient == 0) {
    return 0.0f;
  }
  return static_cast<float>(static_cast<int32_t>(raw)) / static_cast<float>(coefficient);
}

}  // namespace cse7761_core
