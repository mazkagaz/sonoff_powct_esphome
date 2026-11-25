#pragma once

#include <cstdint>

namespace cse7761_core {

constexpr double kStockK1 = 0.2033;

inline uint32_t stock_voltage_mv(uint32_t raw, uint32_t coefficient) {
  if (raw >= 0x800000U || coefficient == 0) {
    return 0;
  }
  const uint64_t intermediate =
      static_cast<uint64_t>(raw) * static_cast<uint64_t>(coefficient) * 10ULL;
  return static_cast<uint32_t>(((intermediate >> 21) + 1U) >> 1U);
}

inline uint32_t stock_current_ma(uint32_t raw, uint32_t coefficient) {
  if (raw >= 0x800000U || coefficient == 0) {
    return 0;
  }
  const double scaled = static_cast<double>(raw) * static_cast<double>(coefficient) / kStockK1;
  const uint64_t intermediate = static_cast<uint64_t>(scaled);
  return static_cast<uint32_t>(((intermediate >> 22) + 1U) >> 1U);
}

inline uint32_t stock_power_mw(uint32_t raw, uint32_t coefficient) {
  if (coefficient == 0) {
    return 0;
  }
  const int32_t signed_raw = static_cast<int32_t>(raw);
  const uint32_t magnitude = signed_raw < 0
                                 ? static_cast<uint32_t>(-static_cast<int64_t>(signed_raw))
                                 : raw;
  const double scaled = static_cast<double>(magnitude) * static_cast<double>(coefficient) *
                        1000.0 / kStockK1;
  const uint64_t intermediate = static_cast<uint64_t>(scaled);
  return static_cast<uint32_t>(((intermediate >> 30) + 1U) >> 1U);
}

}  // namespace cse7761_core
