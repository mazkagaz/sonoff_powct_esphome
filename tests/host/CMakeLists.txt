cmake_minimum_required(VERSION 3.16)
project(powct_host_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(host_tests
    test_smoke.cpp
    test_protocol.cpp
    test_conversion.cpp
    test_calibration.cpp
    test_stock_calibration.cpp
    test_stock_conversion.cpp
    test_calibration_payload.cpp
    test_calibration_status.cpp
    test_golden_measurements.cpp
    test_energy.cpp
)
target_include_directories(host_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/../../components/cse7761
)
target_compile_definitions(host_tests PRIVATE
    GOLDEN_MEASUREMENTS_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../golden/measurements.csv"
)
target_compile_options(host_tests PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME host_tests COMMAND host_tests)
