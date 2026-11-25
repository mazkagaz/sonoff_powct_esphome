# ADR 0001 — Use doctest for host tests

**Date:** 2026-11-25  
**Status:** Accepted

## Context

The project needs fast, deterministic unit tests for protocol parsing,
conversion, calibration, filtering and energy logic. The tests run on a host
(Linux, C++17) and must be easy to run locally and in GitHub Actions.

## Decision

Use **doctest** as the host test framework.

- Header-only and easy to vendor or fetch with CMake.
- Fast compile time and small footprint.
- `TEST_CASE`, `CHECK`, `REQUIRE` and `SUBCASE` cover the expected needs.
- CTest integration is straightforward.

## Consequences

- The test suite is lightweight and fast in CI.
- Advanced matchers and generators from Catch2 are not available; if the suite
  grows to need them, migration is possible but not free.
- The framework version is pinned in `tests/host/CMakeLists.txt`.
