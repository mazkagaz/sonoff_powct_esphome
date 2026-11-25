#include <doctest/doctest.h>

#include "cse7761_core_calibration.h"

using namespace cse7761_core;

TEST_CASE("calibration offsets follow the current and power scales") {
    CalibrationOffsets offsets{0.0, 0.0, 0.0, 0.0};
    const auto result = apply_calibration_offsets(offsets, 200.0, 400.0, 20);

    CHECK(result.current_b == doctest::Approx(-10.0));
    CHECK(result.power_b == doctest::Approx(-20.0));
    CHECK(result.current_a == doctest::Approx(-104.6));
    CHECK(result.power_a == doctest::Approx(177.8));
}

TEST_CASE("calibration preserves existing offsets before applying sums") {
    CalibrationOffsets offsets{1.0, 2.0, 3.0, 4.0};
    const auto result = apply_calibration_offsets(offsets, 20.0, 40.0, 20);

    CHECK(result.current_b == doctest::Approx(1.0));
    CHECK(result.power_b == doctest::Approx(2.0));
    CHECK(result.current_a == doctest::Approx(10.46));
    CHECK(result.power_a == doctest::Approx(-17.78));
}

TEST_CASE("zero samples leave offsets unchanged") {
    CalibrationOffsets offsets{1.0, 2.0, 3.0, 4.0};
    const auto result = apply_calibration_offsets(offsets, 20.0, 40.0, 0);

    CHECK(result.current_b == doctest::Approx(2.0));
    CHECK(result.power_b == doctest::Approx(4.0));
    CHECK(result.current_a == doctest::Approx(1.0));
    CHECK(result.power_a == doctest::Approx(3.0));
}
