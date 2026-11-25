#include <doctest/doctest.h>

#include "cse7761_core_stock_calibration.h"

using namespace cse7761_core;

TEST_CASE("identity calibration keeps the converted values") {
    const auto calibration = identity_stock_calibration();
    CHECK(calibration.valid);
    CHECK(apply_quadratic(calibration.voltage[0], 230000.0) == doctest::Approx(230000.0));
    CHECK(apply_quadratic(calibration.current[0], 5000.0) == doctest::Approx(5000.0));
    CHECK(apply_quadratic(calibration.power[0], 1150000.0) == doctest::Approx(1150000.0));
}

TEST_CASE("quadratic calibration applies gain offset and curvature") {
    const QuadraticCoefficients coefficients{10000.0, 1000000.0, 1000000.0};
    // 10000*1000^2/1e7 + 1000000*1000/1e6 + 1000000/1e6 = 1000 + 1000 + 1
    CHECK(apply_quadratic(coefficients, 1000.0) == doctest::Approx(2001.0));
}

TEST_CASE("quadratic calibration truncates and clamps negative results") {
    CHECK(apply_quadratic({0.0, 1.0, 0.0}, 1.0) == doctest::Approx(0.0));
    CHECK(apply_quadratic({0.0, -1000000.0, 0.0}, 1000.0) == doctest::Approx(0.0));
}

TEST_CASE("stock range switches at 500 mA") {
    CHECK(select_stock_range(0.0) == 0);
    CHECK(select_stock_range(499.0) == 0);
    CHECK(select_stock_range(500.0) == 1);
    CHECK(select_stock_range(500.1) == 1);
}

TEST_CASE("stock calibration applies the selected range") {
    auto calibration = identity_stock_calibration();
    calibration.current[0] = {0.0, 2000000.0, 0.0};
    calibration.current[1] = {0.0, 3000000.0, 0.0};

    CHECK(apply_stock_calibration(calibration, 230000.0, 1000.0, 230000.0, 0).current_ma ==
          doctest::Approx(2000.0));
    CHECK(apply_stock_calibration(calibration, 230000.0, 1000.0, 230000.0, 1).current_ma ==
          doctest::Approx(3000.0));
}

TEST_CASE("stock parser accepts the default eWeLink payload") {
    const std::string payload =
        ",1,[0,1000000,0,0,1000000,0,0,1000000,0],"
        "[0,1000000,0,0,1000000,0,0,1000000,0]";
    StockCalibration calibration;
    CHECK(parse_stock_calibration(payload, calibration));
    CHECK(calibration.valid);
    CHECK(apply_quadratic(calibration.voltage[0], 230000.0) == doctest::Approx(230000.0));
    CHECK(apply_quadratic(calibration.power[1], 1150000.0) == doctest::Approx(1150000.0));
}

TEST_CASE("stock parser accepts negative and decimal coefficients") {
    const std::string payload =
        "1,1,[1,1000001,-2,3,1000003,4,5,1000005,6],"
        "[7,1000007,-8,9,1000009,10,11,1000011,12]";
    StockCalibration calibration;
    CHECK(parse_stock_calibration(payload, calibration));
    CHECK(calibration.valid);
    CHECK(calibration.current[0].c == doctest::Approx(-2.0));
    CHECK(calibration.current[1].c == doctest::Approx(-8.0));
    CHECK(calibration.power[1].c == doctest::Approx(12.0));
}

TEST_CASE("stock parser rejects malformed payloads") {
    StockCalibration calibration;
    CHECK_FALSE(parse_stock_calibration("", calibration));
    CHECK_FALSE(parse_stock_calibration("[0,1000000,0]", calibration));
    CHECK_FALSE(parse_stock_calibration("[0,1000000,0,0,1000000,0]", calibration));
    CHECK_FALSE(parse_stock_calibration("[0,1000000,0,0,1000000,0,0,1000000,0]", calibration));
}
