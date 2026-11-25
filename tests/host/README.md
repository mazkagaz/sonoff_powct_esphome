# Host tests

The host suite uses **doctest** and CMake/CTest. It runs on a Linux host and
does not need ESPHome or an ESP32.

```bash
cmake -S tests/host -B build/host
cmake --build build/host --parallel
ctest --test-dir build/host --output-on-failure
```

Phase 1 contains a toolchain smoke test. Phase 2 adds tests for the extracted
`components/cse7761/core/` modules:

- protocol framing and checksums;
- stock conversions and ranges;
- quadratic calibration and fallback;
- filter, energy and error handling.

Test data such as recorded UART frames belongs in `tests/data/`.
