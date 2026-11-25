# Requirement → Test Matrix

**Status:** Draft v0.1 — 2026-11-25  
**Legend:** Planned = test specified but not implemented; Partial = some coverage;
Pass = green in CI/HIL; N/A = not applicable.

| Requirement | Test IDs | Level | Status |
|---|---|---|---|
| REQ-MEAS-001 | UNIT-CONV-001, FUNC-MEAS-001, HIL-MEAS-001 | Unit/Functional/HIL | Partial |
| REQ-MEAS-002 | UNIT-ENERGY-001, FUNC-ENERGY-001, HIL-EXPORT-001 | Unit/Functional/HIL | Partial |
| REQ-MEAS-003 | UNIT-CAL-004, FUNC-CONFIG-002 | Unit/Functional | Planned |
| REQ-MEAS-004 | UNIT-ENERGY-002, HIL-ZERO-001 | Unit/HIL | Planned |
| REQ-MEAS-005 | FUNC-CADENCE-001, HIL-SOAK-001 | Functional/HIL | Planned |
| REQ-CAL-001 | UNIT-CONV-001..003 | Unit | Partial |
| REQ-CAL-002 | UNIT-CAL-001..003, HIL-RANGE-001 | Unit/HIL | Planned |
| REQ-CAL-003 | UNIT-CAL-005, FUNC-CONFIG-003 | Unit/Functional | Planned |
| REQ-ENERGY-001 | UNIT-ENERGY-001, HIL-REBOOT-001 | Unit/HIL | Partial |
| REQ-COMM-001 | UNIT-PROTO-001..004 | Unit | Partial |
| REQ-COMM-002 | UNIT-ERR-001..003 | Unit | Planned |
| REQ-API-001 | FUNC-CONFIG-001 | Functional | Planned |
| REQ-SEC-001 | FUNC-SEC-001, SEC-SCAN-001 | Functional/CI | Planned |
| REQ-CONFIG-001 | FUNC-CONFIG-001..004 | Functional | Planned |
| REQ-SAFE-001 | DOC-CHECK-001 | Docs | Planned |
| REQ-DOC-001 | DOC-CHECK-001, PR-CHECK-001 | Docs/Process | Partial |
| REQ-TEST-001 | CI-HOST-001, PR-CHECK-001 | CI/Process | Partial |
| NFR-ACC-001 | REF-ACC-001..006 | Reference/manual | Pending — NOT EXECUTED |
| NFR-REL-001 | HIL-SOAK-001 | HIL | Planned |
| NFR-COMP-001 | CI-ESPHOME-001 | CI | Partial |
| NFR-PERF-001 | HIL-CADENCE-001 | HIL | Planned |
| NFR-SEC-001 | SEC-SCAN-001 | CI | Planned |
| NFR-MAINT-001 | UNIT-* | Unit | Partial |

## Current baseline

- 21 doctest cases pass in the host suite.
- Protocol, conversion, calibration and energy headers are extracted and covered.
- `esphome config` passes for both YAML files.
- The component compiles with ESPHome 2026.11.1 and ESP-IDF 5.5.6 (selected by
  ESPHome); the historical `5.4.0` pin has been removed.
- Manual reference validation (POW CT vs Shelly EM) is documented but
  **NOT EXECUTED**: no measurements have been collected yet.
