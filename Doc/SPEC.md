# Sonoff POW CT ESPHome — Specification

**Status:** Draft v0.1 — 2026-11-25  
**Scope:** standard Sonoff POW CT with CSE7761, ESPHome component, Home Assistant integration.  
**Language:** engineering documents are English; user guides remain English/French.

## 1. Purpose

Define the measurable behaviour of the firmware so that implementation, tests and
documentation share one reference. Every requirement has an ID and is traced to
one or more tests in [TEST_MATRIX.md](TEST_MATRIX.md).

## 2. Scope

In scope:

- voltage, current and active power on channel A;
- optional channel B used as a bias reference;
- signed power/energy for import and export;
- accumulated energy and persistence;
- CSE7761 UART protocol, calibration, filtering and error handling;
- Home Assistant actions and encrypted OTA;
- local and remote YAML configurations.

Out of scope: a second physical CT without hardware modification, official
support for the Ring 100 A variant, and a full energy-meter certification.

## 3. Definitions

- **Channel A:** the CT channel exposed on the standard POW CT.
- **Channel B:** the second CSE7761 current channel; not a second CT on the
  standard board. Current firmware uses it as a bias reference.
- **Range:** low/high current range selected from the converted current.
- **Import/export:** signed power direction; positive is consumption, negative is
  production.
- **Stock:** behaviour observed in the original Sonoff firmware.

## 4. Functional requirements

| ID | Requirement | Acceptance criteria | Status |
|---|---|---|---|
| REQ-MEAS-001 | Measure RMS voltage, channel A RMS current and channel A active power. | Values published in V, A and W with the configured precision; invalid readings do not publish zero. | Current |
| REQ-MEAS-002 | Provide signed power and separate import/export energy. | Positive power = import, negative = export; energy counters increase only in their direction. | Current |
| REQ-MEAS-003 | Use channel B only as a bias reference on the standard board. | Channel B is not advertised as a second CT; bias correction can be disabled for stock calibration. | Current |
| REQ-MEAS-004 | Respect an anti-creep threshold. | Below the configured threshold (stock: about 2 W), active power is reported as zero. | Planned / stock parity |
| REQ-MEAS-005 | Run measurement at 80 ms, publish at 1 s and persist energy hourly. | Cadence is configurable; defaults match the requirement; no blocking loop. | Planned |
| REQ-CAL-001 | Apply the stock conversion constants and units. | Raw registers are converted to mV/mA/mW using the stock formulas and `0.2033` K1 value. | Planned / stock parity |
| REQ-CAL-002 | Apply 18 quadratic coefficients across two ranges. | Six groups of `(a,b,c)` for current, voltage and power, low/high range; high range when converted current ≥ 500 mA; negative results clamp to zero. | Planned |
| REQ-CAL-003 | Fall back safely when calibration is absent or invalid. | Identity calibration `[0,1e6,0]` is used; malformed data is rejected and logged, never applied partially. | Planned |
| REQ-ENERGY-001 | Integrate filtered, calibrated power into energy. | Energy uses the same filtered value as publication; reboot restores the last persisted value. | Current / to verify |
| REQ-COMM-001 | Communicate with the CSE7761 over UART. | 38400 baud, 8E1, GPIO25 RX / GPIO26 TX; `0xA5` frames; checksum over all data bytes. | Current |
| REQ-COMM-002 | Survive UART errors. | Timeout per transaction; keep last good value; after 100 consecutive failures publish/zero only according to the documented policy. | Current / to verify |
| REQ-API-001 | Expose register read/write actions to Home Assistant. | YAML uses `api.actions` and `action:`; write action field is `register_value`; action names remain `read_register`/`write_register`. | Current |
| REQ-SEC-001 | Protect API and OTA. | API encryption key required; encrypted OTA inherits the API key via `ota.encryption:`; secrets are never committed. | Current |
| REQ-CONFIG-001 | Provide local and remote YAML examples. | Both compile with `esphome config`; local example documents `my_components`; remote example uses the GitHub component source. | Current |
| REQ-SAFE-001 | Document safe installation and limitations. | Flashing guide states mains/3.3 V safety and one-CT limitation; Ring is not promised. | Current |
| REQ-DOC-001 | Keep engineering and user documentation current. | Engineering docs are English; user guides are EN/FR; every behaviour change updates spec/design/test/changelog. | Current |
| REQ-TEST-001 | Protect against regressions. | Every behaviour change adds or updates automated tests; CI must pass before merge. | Planned |

## 5. Non-functional requirements

| ID | Requirement | Acceptance criteria |
|---|---|---|
| NFR-ACC-001 | Measurement accuracy (provisional target). | Within ±1 % of reading from 10 % to 100 % of range and ±2 % below 10 %, against a reference meter; to be validated on the bench. |
| NFR-REL-001 | Stable operation across reboot and network loss. | No crash or watchdog reset over a 72 h soak; energy survives reboot. |
| NFR-COMP-001 | Build compatibility. | Compiles with the ESPHome version pinned in `requirements-dev.txt` and the ESP-IDF framework it recommends; no manual ESP-IDF pin. |
| NFR-PERF-001 | Responsive device. | UART and measurement work do not block the ESPHome loop; publish cadence is met. |
| NFR-SEC-001 | Secret hygiene. | `secrets.yaml` is ignored; CI secret scanning finds no credentials. |
| NFR-MAINT-001 | Testable architecture. | Pure protocol/conversion/calibration/energy logic is host-testable without ESPHome. |

## 6. Out of scope

- Two-channel measurement on an unmodified standard POW CT.
- Guaranteed support for Ring 100 A or other hardware revisions.
- Legal/metrology certification.
- Spanish documentation.

## 7. Open questions

- Exact stock filter and range-transition details (partially inferred).
- Factory calibration values and NVS layout on erased devices.
- Final accuracy target and reference meter.
- Whether channel B should be exposed as an optional second measurement on a
  modified board.

## 8. Traceability

The mapping from requirement to test is maintained in
[TEST_MATRIX.md](TEST_MATRIX.md). A requirement without a test is not considered
done.
