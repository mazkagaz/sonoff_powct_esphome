#include <doctest/doctest.h>

#include "cse7761_core_calibration_status.h"

using namespace cse7761_core;

TEST_CASE("offset mode reports offset") {
    CHECK(calibration_status_text(false, StockCalibrationProbe::Pending) == "offset");
    CHECK(calibration_status_text(false, StockCalibrationProbe::Found) == "offset");
}

TEST_CASE("stock mode reports pending before the NVS probe") {
    CHECK(calibration_status_text(true, StockCalibrationProbe::Pending) == "stock: pending");
}

TEST_CASE("stock mode reports the found NVS namespace and key") {
    CHECK(calibration_status_text(true, StockCalibrationProbe::Found, "nvs",
                                  "calib_coefficient0") ==
          "stock: nvs/calib_coefficient0");
}

TEST_CASE("stock mode reports identity fallback reasons") {
    CHECK(calibration_status_text(true, StockCalibrationProbe::NotFound) ==
          "stock: identity (not found)");
    CHECK(calibration_status_text(true, StockCalibrationProbe::InvalidPayload) ==
          "stock: identity (invalid payload)");
}

TEST_CASE("stock mode reports an unavailable NVS") {
    CHECK(calibration_status_text(true, StockCalibrationProbe::NvsUnavailable) ==
          "stock: NVS unavailable");
}
