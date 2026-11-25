#pragma once

#include <cstdint>

namespace cse7761_core {

enum Measure : uint8_t {
  RMS_IAC,
  RMS_IBC,
  RMS_UC,
  POWER_PAC,
  POWER_PBC,
  POWER_SC,
  ENERGY_AC,
  ENERGY_BC,
};

constexpr uint8_t kFrameStart = 0xA5;

}  // namespace cse7761_core
