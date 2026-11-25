# Contributing

## Principles

- Every behaviour change has a requirement, a test, and a changelog entry.
- Engineering documents are English; user-facing guides are English/French.
- `main` is protected: changes go through a pull request with green CI.
- Never commit `secrets.yaml`, API keys, OTA keys or real Wi-Fi credentials.

## Workflow

1. Open an issue or describe the change in the PR.
2. Create a branch: `feat/...`, `fix/...`, `docs/...`, `test/...`, `refactor/...`.
3. Update the relevant documents **before or with** the code:
   - [Doc/SPEC.md](Doc/SPEC.md) if requirements change;
   - [Doc/DESIGN.md](Doc/DESIGN.md) if architecture or data flow changes;
   - [Doc/TEST_PLAN.md](Doc/TEST_PLAN.md) / [Doc/TEST_MATRIX.md](Doc/TEST_MATRIX.md)
     for new or changed tests;
   - user guides in English and French for user-visible changes;
   - a changelog fragment in `changelog.d/`.
4. Add or update tests.
5. Run the checks locally.
6. Open a PR using the template; wait for CI and review.

## Definition of Done

A PR is done only when:

- [ ] The requirement and acceptance criteria are clear.
- [ ] Code builds with the pinned toolchain.
- [ ] Unit and/or functional tests cover the change.
- [ ] `ctest` passes locally.
- [ ] `esphome config` passes for affected YAML examples.
- [ ] Engineering docs are updated (spec/design/test).
- [ ] User docs are updated in English and French when behaviour or usage
      changes.
- [ ] A changelog fragment exists for user-visible, configuration or behaviour
      changes.
- [ ] No secret is present; `secrets.yaml` is ignored.
- [ ] CI is green.

Docs-only PRs may skip code tests, but must pass the documentation link check.

## Prerequisites

The project is developed on Linux. The versions below are the ones used for the
current validation; newer compatible versions are acceptable.

| Dependency | Minimum / tested | Why |
|---|---|---|
| Git | 2.47.3 tested | Repository and ESPHome external components |
| Python | 3.13.5 tested, with `venv` and `pip` | ESPHome and test tooling |
| CMake + CTest | 3.16 minimum, 3.31.6 tested | Host test build |
| C++17 compiler | GCC 14.2 / Clang | Host tests and doctest |
| Make or Ninja | GNU Make 4.4.1 tested | CMake generator |
| ESPHome | 2026.11.1 pinned in `requirements-dev.txt` | Config validation and compile |
| ESP-IDF | 5.5.6, selected by ESPHome | Build framework; no manual YAML pin |
| esptool / espefuse | optional, bundled with ESPHome | Flashing and diagnostics |

The authoritative list, verification date and update procedure are in
[Doc/VERSIONS.md](Doc/VERSIONS.md).

On Debian/Ubuntu, the system packages are:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git python3 python3-venv python3-pip
```

The host tests use the vendored doctest header; no network access is needed for
CMake configure. ESPHome downloads components and toolchains when needed, so
network access is required for those first runs.

## Local toolchain

The project uses a local Python virtual environment; `.venv/` is ignored by Git.

```bash
python3 -m venv .venv
. .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install -r requirements-dev.txt
esphome version
```

For the local YAML, copy `components/cse7761` to `my_components/cse7761` (or use
the remote YAML). Create `secrets.yaml` from `secrets.yaml.example` before
running `esphome config`.

## Tool and version updates

When upgrading ESPHome, ESP-IDF, Python, CMake, the compiler or doctest:

1. update `requirements-dev.txt` and, for doctest, the vendored header and
   license in `tests/host/third_party/doctest/`;
2. update [Doc/VERSIONS.md](Doc/VERSIONS.md) and the validated-environment note
   in [Doc/INSTALL.md](Doc/INSTALL.md) / [Doc/INSTALL_FR.md](Doc/INSTALL_FR.md);
3. run the host tests, `esphome config` on both YAML files and a real compile;
4. add a changelog fragment.

Do not leave a guide claiming a version that was not actually verified.

## Tests

```bash
cmake -S tests/host -B build/host
cmake --build build/host --parallel
ctest --test-dir build/host --output-on-failure
```

- Host tests use **doctest** (see [Doc/ADR/0001-doctest.md](Doc/ADR/0001-doctest.md)).
- Pure logic belongs in `components/cse7761/cse7761_core_*.h` so it can be
  tested on the host without ESPHome. ESPHome copies only component-root files,
  so do not put these headers in a subdirectory.
- Functional tests use a fake UART transport and recorded frames.
- Hardware-in-the-loop tests follow [Doc/TEST_PLAN.md](Doc/TEST_PLAN.md); they are
  required before a release that changes measurement.

## Changelog fragments

Add one file per PR in `changelog.d/`:

```
changelog.d/<type>-<short-slug>.md
```

`type` is one of `added`, `changed`, `fixed`, `removed`, `security`. Write a
short user-facing sentence. At release time the fragments are assembled into
[CHANGELOG.md](CHANGELOG.md). Do not edit `CHANGELOG.md` directly for routine PRs
unless the change is part of a release.

## Secrets

- Copy [secrets.yaml.example](secrets.yaml.example) to `secrets.yaml` and fill in
  the values.
- `secrets.yaml` is ignored by Git; never force-add it.
- CI uses dummy secrets and must not need repository secrets.
- Report a leaked credential immediately and rotate it.

## Commit style

- Use short imperative subjects, for example `fix: keep last good UART value`.
- Explain the why in the body when it is not obvious.
- Reference the issue or requirement when applicable.
- No generated attribution or unrelated files in the commit.

## Review checklist

- Does the change preserve the single-CT limitation?
- Are units and signed import/export semantics unchanged or explicitly updated?
- Are error paths tested (timeout, checksum, short frame, 100 failures)?
- Are calibration/range boundaries covered?
- Are secrets and user data absent from logs?
- Are docs and changelog updated?
