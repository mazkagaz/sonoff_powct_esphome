#include <doctest/doctest.h>

#include "cse7761_core_energy.h"

using namespace cse7761_core;

TEST_CASE("positive mean power increases received energy") {
    const auto result = integrate_energy({0.0, 0.0}, 100.0, 200.0, 3600.0);
    CHECK(result.received == doctest::Approx(150.0));
    CHECK(result.exported == doctest::Approx(0.0));
}

TEST_CASE("negative mean power increases exported energy") {
    const auto result = integrate_energy({0.0, 0.0}, -100.0, -200.0, 3600.0);
    CHECK(result.received == doctest::Approx(0.0));
    CHECK(result.exported == doctest::Approx(150.0));
}

TEST_CASE("zero mean power changes neither counter") {
    const auto result = integrate_energy({5.0, 7.0}, 0.0, 0.0, 3600.0);
    CHECK(result.received == doctest::Approx(5.0));
    CHECK(result.exported == doctest::Approx(7.0));
}

TEST_CASE("elapsed time uses unsigned millisecond arithmetic across wrap") {
    CHECK(elapsed_seconds(10, 0xFFFFFFF0U) == doctest::Approx(0.026));
}

TEST_CASE("save interval handles first save and millis wrap") {
    CHECK(should_save_energy(1000, 0, 3600000U));
    CHECK_FALSE(should_save_energy(1000, 1000, 3600000U));
    CHECK(should_save_energy(3601000, 1000, 3600000U));
    CHECK(should_save_energy(100, 0xFFFFFF00U, 100U));
}
