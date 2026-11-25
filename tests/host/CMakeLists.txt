cmake_minimum_required(VERSION 3.16)
project(powct_host_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

add_executable(host_tests
    test_smoke.cpp
)
target_link_libraries(host_tests PRIVATE doctest::doctest)
target_compile_options(host_tests PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME host_tests COMMAND host_tests)
