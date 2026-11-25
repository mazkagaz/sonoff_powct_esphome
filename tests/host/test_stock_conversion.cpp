#include <doctest/doctest.h>

#include "cse7761_core_stock_conversion.h"

using namespace cse7761_core;

TEST_CASE("stock voltage conversion follows the integer formula") {
    CHECK(stock_voltage_mv(10000, 42563) == 1015);
    CHECK(stock_voltage_mv(230000, 42563) == 23340);
    CHECK(stock_voltage_mv(0x800000, 42563) == 0);
    CHECK(stock_voltage_mv(1000, 0) == 0);
}

TEST_CASE("stock current conversion follows the 0.2033 formula") {
    CHECK(stock_current_ma(1000, 52241) == 31);
    CHECK(stock_current_ma(5000, 52241) == 153);
    CHECK(stock_current_ma(0x800000, 52241) == 0);
    CHECK(stock_current_ma(1000, 0) == 0);
}

TEST_CASE("stock power conversion uses the signed 32-bit magnitude") {
    CHECK(stock_power_mw(100000, 44513) == 10196);
    CHECK(stock_power_mw(1000000, 44513) == 101958);
    CHECK(stock_power_mw(static_cast<uint32_t>(-100000), 44513) == 10196);
    CHECK(stock_power_mw(0xFFFFFFFF, 44513) == 0);
    CHECK(stock_power_mw(100000, 0) == 0);
}
