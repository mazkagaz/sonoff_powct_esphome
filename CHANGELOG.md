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
- Pull request template and CI workflow for documentation links and host tests.

### Changed

- API actions use `actions:` / `action:`; `register_value` replaces the legacy
  `regiter_value` typo field.
- OTA uses `encryption:` inherited from the API key instead of an OTA password.
- YAML examples use documented substitutions and `!secret` placeholders.
- Documentation uses English/French language selectors; Spanish docs removed.

### Fixed

- Flashing documentation no longer suggests an app-only image without the
  factory/app-only distinction.
- Sensor IDs `w_sensor`/`a_sensor` corrected to `w_sensor_1`/`a_sensor_1` in the
  PF lambda and display.
- Single-CT limitation clarified in README and configuration docs.

### Removed

- Spanish documentation (`README_ES.md`, `Doc/INSTALL_ES.md`,
  `Doc/CALIBRATION_ES.md`).

## Released versions

No tagged release yet. The first release will use `vX.Y.Z` and a dated section.
