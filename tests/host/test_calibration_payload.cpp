#include <doctest/doctest.h>

#include "cse7761_core_calibration_payload.h"
#include "cse7761_core_stock_calibration.h"

using namespace cse7761_core;

TEST_CASE("extract calibration payload from a JSON content blob") {
    const std::string blob =
        "{\"init\":\"true\",\"calib_coefficient0\":\"1,1,"
        "[1,1000001,2,3,1000003,4,5,1000005,6],"
        "[7,1000007,8,9,1000009,10,11,1000011,12]\"}";
    std::string payload;
    REQUIRE(extract_calibration_payload(blob, payload));
    CHECK(payload ==
          "1,1,[1,1000001,2,3,1000003,4,5,1000005,6],"
          "[7,1000007,8,9,1000009,10,11,1000011,12]");

    StockCalibration calibration;
    CHECK(parse_stock_calibration(payload, calibration));
    CHECK(calibration.current[0].b == doctest::Approx(1000001.0));
}

TEST_CASE("extract calibration payload with whitespace") {
    const std::string blob =
        "{ \"calib_coefficient0\" : "
        "\"1,1,[0,1000000,0],[0,1000000,0]\" }";
    std::string payload;
    REQUIRE(extract_calibration_payload(blob, payload));
    CHECK(payload == "1,1,[0,1000000,0],[0,1000000,0]");
}

TEST_CASE("direct calibration string is accepted as a payload") {
    const std::string direct = "1,1,[0,1000000,0],[0,1000000,0]";
    std::string payload;
    REQUIRE(extract_calibration_payload(direct, payload));
    CHECK(payload == direct);
}

TEST_CASE("content without calibration key is rejected") {
    const std::string blob = "{\"init\":\"true\",\"other\":\"value\"}";
    std::string payload;
    CHECK_FALSE(extract_calibration_payload(blob, payload));
}
