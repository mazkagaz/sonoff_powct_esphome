#pragma once

#include <cstdint>
#include <string>

namespace cse7761_core {

enum class StockCalibrationProbe : uint8_t {
  Pending,
  Found,
  NotFound,
  InvalidPayload,
  NvsUnavailable,
};

inline std::string calibration_status_text(bool stock_enabled,
                                           StockCalibrationProbe probe,
                                           const std::string &nvs_namespace = "",
                                           const std::string &nvs_key = "") {
  if (!stock_enabled) {
    return "offset";
  }

  switch (probe) {
    case StockCalibrationProbe::Pending:
      return "stock: pending";
    case StockCalibrationProbe::Found:
      return "stock: " + nvs_namespace + "/" + nvs_key;
    case StockCalibrationProbe::NotFound:
      return "stock: identity (not found)";
    case StockCalibrationProbe::InvalidPayload:
      return "stock: identity (invalid payload)";
    case StockCalibrationProbe::NvsUnavailable:
      return "stock: NVS unavailable";
  }
  return "stock: pending";
}

}  // namespace cse7761_core
