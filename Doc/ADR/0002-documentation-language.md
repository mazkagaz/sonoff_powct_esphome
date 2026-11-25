# ADR 0002 — Documentation language policy

**Date:** 2026-11-25  
**Status:** Accepted

## Context

The project has English and French user guides. Maintaining bilingual
engineering documents (specification, design, test plan, changelog) doubles the
maintenance burden and creates drift.

## Decision

- Engineering documents are written in **English**: `SPEC.md`, `DESIGN.md`,
  `TEST_PLAN.md`, `TEST_MATRIX.md`, ADRs and `CHANGELOG.md`.
- User-facing guides remain **English and French**: installation, flashing,
  calibration, configuration and debug.
- Code comments and commit messages are English.

## Consequences

- A single source of truth for engineering decisions.
- French users keep localized installation and usage documentation.
- Translations of engineering documents are out of scope unless explicitly
  requested.
