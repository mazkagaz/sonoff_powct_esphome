# Host tests

The host suite uses **doctest** (vendored in `third_party/doctest/`) and
CMake/CTest. It runs on a Linux host and does not need ESPHome, network access or
an ESP32.

```bash
cmake -S tests/host -B build/host
cmake --build build/host --parallel
ctest --test-dir build/host --output-on-failure
```

Phase 1 contains a toolchain smoke test. Phase 2 adds tests for the extracted
`components/cse7761/cse7761_core_*` headers:

- protocol framing and checksums;
- stock conversions and ranges;
- quadratic calibration, stock NVS parser and fallback;
- stock conversion, stock calibration and range selection;
- calibration-status text mapping for offset/stock/NVS outcomes;
- the production offset golden measurement pipeline;
- filter, energy and error handling.

The golden test reads `../golden/measurements.csv`, compares production and stock
outputs, and computes drift. Above 2 % drift, `drift_approved_by` and
`drift_approval_reason` are required; expected-value changes require
`approved_by`/`approval_reason` and a manual review. Rows are code-generated
baselines, not physical captures.

Test data such as recorded UART frames belongs in `tests/data/`.
