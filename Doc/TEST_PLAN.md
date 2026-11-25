# Sonoff POW CT ESPHome — Test Plan

**Status:** Draft v0.1 — 2026-11-25  
**Related:** [SPEC.md](SPEC.md), [DESIGN.md](DESIGN.md), [TEST_MATRIX.md](TEST_MATRIX.md)

## 1. Objectives

- Prevent measurement, protocol, calibration and energy regressions.
- Make every requirement verifiable.
- Separate deterministic host tests from hardware-dependent validation.
- Define a release gate that is realistic for a one-person project.

## 2. Test levels

| Level | Scope | Runs | Tooling |
|---|---|---|---|
| Unit | Pure protocol, conversion, calibration, filter, energy | Every PR | doctest + CMake/CTest, host |
| Functional | Component behaviour with a fake UART; YAML schema and build | Every PR | doctest, `esphome config`, `esphome compile` |
| Reference/manual | Real POW CT + Shelly EM on the same circuit | Not executed | Manual CSV + results summary |
| Integration/HIL | Real POW CT, known loads, OTA, persistence | Not available for now | pytest/pyserial, reference meter |

## 3. Environments

- Host: Linux with Git, Python 3.13 (`venv`/`pip`), CMake/CTest ≥3.16, a C++17
  compiler and Make or Ninja. Exact tested versions are listed in
  [VERSIONS.md](VERSIONS.md).
- ESPHome: 2026.11.1 pinned in `requirements-dev.txt`; ESP-IDF 5.5.6 selected by
  ESPHome (no manual YAML pin). Current stable is checked separately when
  available.
- Network: not required for the host tests (doctest is vendored); required for
  ESPHome component/toolchain downloads.
- Hardware: standard POW CT; optional Ring 100 A only as an exploratory variant.
- Secrets in CI: generated dummy `secrets.yaml`; never real credentials.
- Framework: the YAML does not pin ESP-IDF; ESPHome selects its recommended
  version. Compilation was validated with ESPHome 2026.11.1 and that framework.

## 4. Test data

- Recorded UART frames for valid reads, checksum errors, short frames and
  timeouts.
- Golden conversion/calibration vectors, including range boundaries.
- Calibration datasets with identity, stock-like and malformed coefficients.
- Energy sequences covering import, export, zero and anti-creep.

Test data lives in `tests/data/` and changes require review: a golden-file update
must come with an explanation.

### 5.1 Golden measurement tests

`tests/golden/measurements.csv` feeds raw U/I/P samples through the **production
offset pipeline** and compares the exact micro-unit V/I/P outputs. The baseline
covers 220/240 V, 0–100 A and an export power-sign case. The stock quadratic
pipeline, ranges and parser are covered by unit tests. Any difference fails the
test.

Changing an expected value requires a human validation: the row must retain a
non-empty `approved_by` and `approval_reason`, and the change is reviewed in the
PR. The initial rows are a **baseline generated from the current code**, not physical
measurements. Production rows cannot be captured while the load varies; if one is
captured, it uses `source=prod`. See
[tests/golden/README.md](../tests/golden/README.md).

## 5. Unit test areas

| Area | Representative cases | Test ID prefix |
|---|---|---|
| Protocol | Sync, checksum, register size, endianness, short frame | `UNIT-PROTO` |
| Conversion | Voltage/current/power formulas, truncation, invalid MSB | `UNIT-CONV` |
| Calibration | 18 coefficients, low/high range, 500 mA boundary, negative clamp, fallback, stock NVS parser | `UNIT-CAL` |
| Filter | Median/trimmed mean, reset threshold, short windows | `UNIT-FILTER` |
| Energy | Import/export integration, anti-creep, reboot restore, `millis()` wrap | `UNIT-ENERGY` |
| Errors | Timeout, keep-last-good, 100-failure policy | `UNIT-ERR` |

## 6. Functional tests

- Fake UART feeds single and repeated frames; verify sensor values, error
  handling and recovery.
- `esphome config` on both YAML examples with dummy secrets.
- `esphome compile` for the supported ESPHome versions.
- Validate `api.actions` / `action:` / `register_value` schema.
- Validate encrypted OTA configuration (`ota.encryption:` with API key).
- Validate that no secret is required in the repository and that
  `secrets.yaml.example` contains every referenced key.

## 7. HIL matrix (not executed)

The full HIL bench is not available for now. The manual reference validation is
the only planned hardware comparison; it is **NOT EXECUTED**.


| Case | Load / condition | Acceptance |
|---|---|---|
| Zero load | 0 W, CT installed | No false energy; P below anti-creep is zero |
| Low range | 50 W, 100 W | Within accuracy target; stable |
| Range transition | Around 500 mA | No spike or persistent offset |
| High range | 1000 W, 2000 W, 3000 W | Within accuracy target |
| Export | Reversed CT or generation source | Negative power and export energy increase |
| Reboot | Power cycle | Energy restored; no duplicate/lost hour |
| OTA | Encrypted OTA | Update succeeds; device reconnects |
| Variant | Ring 100 A if available | Documented as exploratory, not a support claim |

### 7.1 Manual reference validation (NOT EXECUTED)

The procedure and data template are ready:

- [REFERENCE_VALIDATION.md](REFERENCE_VALIDATION.md) — safety, setup, cases,
  provisional tolerances and reporting.
- [reference_measurements_template.csv](../tests/hil/reference_measurements_template.csv)
  — one row per load/window.

Status: **NOT EXECUTED**. It remains pending until real measurements and a
results summary exist. This is a manual Shelly EM comparison, not a calibrated
metrology test or a full HIL pass.

## 8. CI

Required jobs:

- `docs`: relative Markdown links.
- `host-tests`: configure, build and run the vendored doctest suite.
- `changelog`: on pull requests, require a `changelog.d/` fragment when code or
  configuration changes.
- `esphome-config`: `esphome config` on both YAML examples with dummy secrets on
  every push and pull request.
- `esphome-compile`: real ESPHome compile on `main` pushes and manual runs, with
  a cache for the ESPHome/ESP-IDF toolchain.
- `secret-scan`: gitleaks scan for committed credentials.

Branch protection should require `docs`, `host-tests`, `changelog`,
`esphome-config` and `secret-scan`. `esphome-compile` is the heavier main/manual
gate and is not required on every PR.

## 9. Coverage and quality gates

- ≥90 % line and branch coverage on `cse7761_core_*` pure logic.
- 100 % branch coverage on checksum, range selection and calibration fallback.
- No decrease in core coverage on a PR.
- HIL results recorded before a release that changes measurement.

Coverage is a guide, not a substitute for meaningful assertions.

## 10. Entry and exit criteria

**Entry:** requirement and design updated; test data available; CI green on the
branch.

**Exit for a release:** unit and functional tests green; HIL matrix executed for
measurement changes; changelog updated; no open critical defect; documentation
links valid.

## 11. Defect management

- Every bug gets a regression test before or with the fix.
- Critical measurement/security defects block release.
- Flaky tests are fixed or quarantined with an owner and an expiry date.

## 12. Traceability

[TEST_MATRIX.md](TEST_MATRIX.md) maps every requirement to tests and status.
