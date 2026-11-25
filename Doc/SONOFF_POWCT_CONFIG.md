**English** | [Français](SONOFF_POWCT_CONFIG_FR.md)

# SONOFF POWCT Config

## GPIO Config:
| Pin    |Function                             |
|:-------|:------------------------------------|
| GPIO00 | Push Button (HIGH = off, LOW = on)  |
| GPIO05 | TM1621 DA                           |
| GPIO13 | Status LED (HIGH = off, LOW = on)   |
| GPIO15 | Wifi_LED                            |
| GPIO17 | TM1621 CS                           |
| GPIO18 | TM1621 WR                           |
| GPIO21 | Relay1                              |
| GPIO23 | TM1621 RD                           |
| GPIO25 | CSE7761 Rx                          |
| GPIO26 | CSE7761 Tx                          |

## Calibration mode

- `calibration_mode: offset` (default): channel-B bias correction; preserves the
  previous behaviour.
- `calibration_mode: stock`: reads the stock calibration payload from NVS in
  read-only mode, applies the 18 quadratic coefficients and the low/high ranges.
  If no valid payload is found, identity coefficients are used and a warning is
  logged.

The stock mode is opt-in; its measurement accuracy is not yet validated on
hardware.

## Measurement channels and CT

| Channel | Use in this firmware |
|:--|:--|
| A | CT input; the only CT on the standard POW CT |
| B | Internal bias reference; not a second CT input |

The CSE7761 has two current channels, but the standard POW CT exposes only one
CT on channel A. Channel B is used by this component to estimate and correct the
offset of channel A. A real two-channel setup requires a second CT, wiring to
channel B, a dedicated calibration, and disabling/rethinking the bias correction.
See [FLASHING.md](FLASHING.md) for hardware variants.

