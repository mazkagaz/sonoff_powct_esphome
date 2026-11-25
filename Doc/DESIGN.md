# Sonoff POW CT ESPHome — Design

**Status:** Draft v0.1 — 2026-11-25  
**Related:** [SPEC.md](SPEC.md), [TEST_PLAN.md](TEST_PLAN.md), [FLASHING.md](FLASHING.md)

## 1. Constraints

- ESP32 + CSE7761, single CT on channel A on the standard POW CT.
- ESPHome component with UART, sensors, preferences and Home Assistant API.
- The original firmware is the reference for measurement parity.
- The device is mains-powered; safety and isolation are non-negotiable.

## 2. Architecture

```
CSE7761 --UART 38400 8E1--> protocol parser --> raw registers
                                                |
                                                v
                                   conversion (stock units)
                                                |
                                                v
                                   calibration / ranges
                                                |
                                                v
                                   filter + energy integration
                                                |
                         +----------------------+------------------+
                         v                                         v
                  ESPHome sensors                        Home Assistant actions
```

### 2.1 Layers

| Layer | Responsibility | Current location | Target |
|---|---|---|---|
| Transport | UART read/write, timeouts | `components/cse7761/cse7761.cpp` | `core/transport` + thin ESPHome adapter |
| Protocol | `0xA5` frames, checksum, register sizes | component | `core/protocol` |
| Conversion | Raw → mV/mA/mW, stock constants | component | `core/conversion` |
| Calibration | Quadratic coefficients, ranges, fallback | component | `core/calibration` |
| Filter/energy | Filtering, integration, anti-creep | component | `core/energy` |
| ESPHome glue | Sensors, preferences, actions, OTA | component | component |

The `core/` extraction is the main testability refactor; it must not change
behaviour.

## 3. Protocol

- UART: 38400 baud, 8 data bits, even parity, 1 stop bit, GPIO25 RX / GPIO26 TX.
- Frames start with `0xA5`; checksum is the one's complement of the sum of all
  bytes after the start byte (including command/data), truncated to 8 bits.
- Register reads return little-endian data of the requested size; 24-bit and
  32-bit values are assembled explicitly.
- Invalid length, checksum or timeout returns an error; callers keep the last
  good value.

Key registers used by the design:

| Register | Purpose |
|---|---|
| `0x00` | SYSCON |
| `0x01` | EMUCON |
| `0x13` | EMUCON2 |
| `0x24` | RMS current A |
| `0x25` | RMS current B |
| `0x26` | RMS voltage |
| `0x2C` | Active power A |
| `0x2D` | Active power B |
| `0x43` | System status |
| `0x70..0x77` | Chip calibration coefficients |

## 4. Measurement pipeline

1. Read raw voltage, current A, power A (and optionally B).
2. Convert to stock units (mV, mA, mW) with the documented constants.
3. Select low/high range from converted current (stock threshold: 500 mA).
4. Apply the 18 quadratic coefficients for the selected range.
5. Clamp negative results to zero where required.
6. Filter the calibrated value for publication and energy.
7. Publish sensors and integrate energy.

The exact stock formulas and coefficient layout are documented in
[CALIBRATION.md](CALIBRATION.md) and the analysis reports. Conversion and
calibration must be pure functions for host tests.

## 5. Calibration

- **Current implementation:** offset-only bias correction using channel B; values
  are held in RAM and are not the stock quadratic model.
- **Target (stock parity):** 18 doubles, six groups of `(a,b,c)` for current,
  voltage and power, low/high range; fallback identity `[0,1e6,0]`; negative
  result clamps to zero.
- Calibration source order: explicit user calibration → stock NVS → identity.
- Channel B is not a second CT; it is an inactive reference. A modified board
  with a second CT needs a separate calibration and a different bias strategy.

## 6. Energy

- Integrate the filtered, calibrated active power over time.
- Keep import and export counters separate; signed power decides the direction.
- Persist periodically (target: hourly) and on controlled shutdown when
  available; restore at boot.
- Apply the anti-creep threshold before integration.

## 7. Error handling

- Every UART transaction has a timeout.
- Keep-last-good on transient errors; count consecutive failures.
- After the documented failure limit, publish/zero according to policy; never
  silently turn a communication error into a valid zero measurement.
- `millis()` wraparound must be handled with unsigned deltas or a 64-bit timer.

## 8. Configuration and security

- YAML uses `api.actions` / `action:`; write action field is `register_value`.
- `ota.encryption:` inherits `api.encryption.key`; no separate OTA password.
- Secrets live in `secrets.yaml`, copied from `secrets.yaml.example`, and are
  ignored by Git.
- The local YAML uses `my_components`; the remote YAML pulls the component from
  GitHub. The difference is documented in [INSTALL.md](INSTALL.md).

## 9. Testability

- Pure `core/` modules with no ESPHome includes.
- Fake UART transport and recorded frames for functional tests.
- Golden files for raw frames and expected converted/calibrated values.
- HIL bench for measurement accuracy, ranges, variants and persistence.

## 10. Open design decisions

- Exact stock filter and range-transition implementation.
- NVS namespace/key discovery for factory coefficients.
- Whether to expose channel B as an optional second measurement on modified
  hardware.
- Whether to keep offset-only calibration as a selectable mode after stock
  parity is implemented.
