#pragma once

#include <cstdint>

namespace cse7761_core {

struct EnergyTotals {
  double received;
  double exported;
};

inline double elapsed_seconds(uint32_t now, uint32_t then) {
  return static_cast<double>(now - then) / 1000.0;
}

inline EnergyTotals integrate_energy(EnergyTotals totals, double previous_power,
                                     double current_power, double delta_seconds) {
  const double mean_power = (previous_power + current_power) / 2.0;
  const double delta_energy = (mean_power * delta_seconds) / 3600.0;

  if (mean_power > 0.0) {
    totals.received += delta_energy;
  } else {
    totals.exported -= delta_energy;
  }
  return totals;
}

inline bool should_save_energy(uint32_t now, uint32_t last_save,
                               uint32_t interval_ms = 3600000U) {
  return last_save == 0 || static_cast<uint32_t>(now - last_save) >= interval_ms;
}

}  // namespace cse7761_core
