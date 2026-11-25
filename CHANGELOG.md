# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project uses
[Semantic Versioning](https://semver.org/).

## [Unreleased]

_Nothing yet._

## [2.2.0] - 2026-11-25

### Added

- English/French flashing guide covering factory vs app-only/OTA images,
  offsets, recovery and diagnostics.
- `secrets.yaml.example` and a Git ignore rule for `secrets.yaml`.
- Engineering documentation: specification, design, test plan, traceability
  matrix and architecture decision records.
- Host test skeleton using doctest and CTest.
- `requirements-dev.txt` and documented host prerequisites (CMake, compiler,
  Python/venv, ESPHome version).
- `Doc/VERSIONS.md` with the validated ESPHome/ESP-IDF/tool versions, plus
  user-facing validated-version notes in the installation guides.
- Host-testable `cse7761_core_*` headers for protocol, conversion, calibration
  and energy, with 42 doctest cases.
- Stock 18-coefficient calibration core and opt-in `calibration_mode: stock`
  pipeline with read-only NVS probe, stock conversion, 500 mA range selection,
  identity fallback and NVS payload parser. `offset` remains the default and
  hardware accuracy validation is pending.
- Golden comparison test between the production offset and stock pipelines, with
  a 2 % drift guard and explicit manual approval metadata. Baseline is
  code-generated, not a physical capture.
- Diagnostic `calibration_status` text sensor exposing the active mode and NVS
  result, useful when the device has no physical access.
- NVS probe now checks `user_unusual`/`user_usual` and the iotgo `content` JSON
  blob for `calib_coefficient0`.
- Pull request template and CI workflow for documentation links and host tests.
- CI jobs for ESPHome config validation, main/manual compile, changelog fragment
  enforcement and gitleaks secret scanning.
- Manual POW CT vs Shelly EM reference-validation procedure and CSV template.
  **Status: NOT EXECUTED; no measurements collected yet.**

### Changed

- API actions use `actions:` / `action:`; `register_value` replaces the legacy
  `regiter_value` typo field.
- OTA uses `encryption:` inherited from the API key instead of an OTA password.
- YAML examples use documented substitutions and `!secret` placeholders.
- Component protocol, conversion, calibration and energy logic now use the
  host-testable core headers.
- Removed the historical ESP-IDF 5.4.0 pin from both YAML files; ESPHome now
  selects its recommended framework version.
- Documentation uses English/French language selectors; Spanish docs removed.

### Fixed

- Flashing documentation no longer suggests an app-only image without the
  factory/app-only distinction.
- Sensor IDs `w_sensor`/`a_sensor` corrected to `w_sensor_1`/`a_sensor_1` in the
  PF lambda and display.
- Single-CT limitation clarified in README and configuration docs.
- Energy timing now uses unsigned millisecond arithmetic, avoiding the
  `millis()` wrap issue.
- A zero conversion coefficient is rejected instead of dividing by zero.
- doctest is vendored in the repository so host tests work with CMake 4 and
  without network access during configure.

### Removed

- Spanish documentation (`README_ES.md`, `Doc/INSTALL_ES.md`,
  `Doc/CALIBRATION_ES.md`).

## [2.1.0] - 2025-12-28

### Changed

- Reversed-engineered the register conversion coefficients: K1 = 4.7, K2 = 1,
  gain 0 on all measurement registers.
- Removed the artificial division-by-pi workaround and the related high-gain
  saturation issue.
- Measurements compared against a Shelly EM reference.

## [2.0.1] - 2025-10-28

### Added

- Signed quantities, calibration and bidirectional energy measurement.

### Changed

- `Energy received` replaces `total_daily_energy`; same result unless the energy
  sensor is treated as an all-time counter.

## [1.0.1] - 2025-10-25

### Added

- YAML configuration example pointing to the GitHub component source.

## [1.0.0] - 2025-10-25

### Added

- Initial stable CSE7761 component, documentation and YAML for the Sonoff POW CT.

[Unreleased]: https://github.com/mazkagaz/sonoff_powct_esphome/compare/v2.2.0...HEAD
[2.2.0]: https://github.com/mazkagaz/sonoff_powct_esphome/compare/v2.1.0...v2.2.0
[2.1.0]: https://github.com/mazkagaz/sonoff_powct_esphome/compare/v2.0.1...v2.1.0
[2.0.1]: https://github.com/mazkagaz/sonoff_powct_esphome/compare/v1.0.1...v2.0.1
[1.0.1]: https://github.com/mazkagaz/sonoff_powct_esphome/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/mazkagaz/sonoff_powct_esphome/releases/tag/v1.0.0
