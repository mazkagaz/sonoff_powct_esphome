#include <doctest/doctest.h>

#include "cse7761_core_conversion.h"

using namespace cse7761_core;

TEST_CASE("coefficient conversion matches the firmware formulas") {
    uint32_t coefficients[8] = {};
    coefficients[RMS_IAC] = 0x1000;
    coefficients[RMS_UC] = 0x2000;
    coefficients[POWER_PAC] = 0x0001;

    CHECK(coefficient_for_unit(RMS_IAC, coefficients) == 435744);
    CHECK(coefficient_for_unit(RMS_UC, coefficients) == 51200);
    CHECK(coefficient_for_unit(POWER_PAC, coefficients) == 456911414);
}

TEST_CASE("a zero coefficient is rejected instead of dividing by zero") {
    uint32_t coefficients[8] = {};
    CHECK(coefficient_for_unit(RMS_IAC, coefficients) == 0);
    CHECK(voltage_from_raw(1000, 0) == doctest::Approx(0.0f));
    CHECK(current_from_raw(1000, 0) == doctest::Approx(0.0f));
    CHECK(power_from_raw(1000, 0) == doctest::Approx(0.0f));
}

TEST_CASE("voltage rejects the invalid MSB and converts valid samples") {
    CHECK(voltage_from_raw(10000, 51200) == doctest::Approx(0.1953125f));
    CHECK(voltage_from_raw(0x800000, 51200) == doctest::Approx(0.0f));
}

TEST_CASE("current uses 24-bit sign extension") {
    CHECK(current_from_raw(500, 435744) == doctest::Approx(0.0011474627f));
    CHECK(current_from_raw(0xFFFFFF, 435744) == doctest::Approx(-0.0000022949255f));
}

TEST_CASE("power uses the signed 32-bit raw value") {
    CHECK(power_from_raw(1000, 456911414) == doctest::Approx(0.0000021887f));
    CHECK(power_from_raw(0xFFFFFFFF, 456911414) == doctest::Approx(-0.0000000021886f));
}
