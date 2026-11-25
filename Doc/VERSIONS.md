# Validated Tool Versions

**Last verified:** 2026-11-25  
**Scope:** versions used to validate the repository, not a promise that newer
versions cannot work.

This file is the source of truth for tool versions. User-facing guides repeat the
ESPHome and ESP-IDF versions so that an unchanged guide still tells a reader
which environment was validated.

| Tool | Version | Notes |
|---|---|---|
| ESPHome | 2026.11.1 | Pinned in `requirements-dev.txt`; config and compile validated |
| ESP-IDF | 5.5.6 | Recommended by ESPHome 2026.11.1; no manual YAML pin |
| Python | 3.13.5 | Project `.venv`, `venv`/`pip` |
| CMake / CTest | 3.31.6 | Host tests |
| GCC | 14.2.0 | Host tests, C++17 |
| GNU Make | 4.4.1 | CMake generator |
| Git | 2.47.3 | Repository and external components |
| doctest | 2.4.11 | Pinned in `tests/host/CMakeLists.txt` |
| esptool | 5.6.0 | Bundled with ESPHome; flashing and diagnostics |
| espefuse | 5.6.0 | Bundled with ESPHome; read-only diagnostics |

## Verification commands

```bash
.venv/bin/esphome version
.venv/bin/esptool version
.venv/bin/espefuse version
python3 --version
cmake --version
g++ --version
git --version
```

The framework version is the one ESPHome selects and downloads; it is visible in
the compile log and in `~/.cache/esphome/idf/frameworks/`.

## Update policy

Review versions at every ESPHome release and at least quarterly. When upgrading:

1. update `requirements-dev.txt` and, if needed, the doctest pin in
   `tests/host/CMakeLists.txt`;
2. update this file and the validated-environment note in
   [INSTALL.md](INSTALL.md) / [INSTALL_FR.md](INSTALL_FR.md);
3. run the host tests, `esphome config` on both YAML files, and a real compile;
4. add a changelog fragment.

If a documented version no longer works, update the docs in the same change
rather than leaving a stale guide.
