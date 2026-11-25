# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project uses
[Semantic Versioning](https://semver.org/).

## [Unreleased]

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
  and energy, with 33 doctest cases.
- Stock 18-coefficient calibration core and opt-in `calibration_mode: stock`
  pipeline with read-only NVS probe, stock conversion, 500 mA range selection,
  identity fallback and NVS payload parser. `offset` remains the default and
  hardware accuracy validation is pending.
- Golden comparison test between the production offset and stock pipelines, with
  a 2 % drift guard and explicit manual approval metadata. Baseline is
  code-generated, not a physical capture.
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

## Released versions

No tagged release yet. The first release will use `vX.Y.Z` and a dated section.
