#!/usr/bin/env python3
"""Require a changelog fragment when code or configuration changes in a PR."""

from __future__ import annotations

import os
import subprocess
import sys

CODE_PREFIXES = (
    "components/",
    "tests/host/",
    ".github/workflows/",
    ".github/scripts/",
)
CODE_FILES = {"requirements-dev.txt"}


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], text=True).strip()


def main() -> int:
    base = os.environ.get("GITHUB_BASE_REF", "").strip()
    if not base:
        print("No pull-request base ref; skipping changelog check.")
        return 0

    subprocess.run(
        ["git", "fetch", "origin", base],
        check=True,
        stdout=subprocess.DEVNULL,
    )
    changed = [
        line
        for line in git("diff", "--name-only", f"origin/{base}...HEAD").splitlines()
        if line
    ]

    code_changed = any(
        path.startswith(CODE_PREFIXES)
        or path in CODE_FILES
        or path.startswith("sonoff_powct_")
        for path in changed
    )
    if not code_changed:
        print("No code or configuration changes; changelog fragment not required.")
        return 0

    fragments = [
        path
        for path in changed
        if path.startswith("changelog.d/")
        and path.endswith(".md")
        and not path.endswith("README.md")
    ]
    if not fragments:
        print("Code or configuration changed without a changelog.d/ fragment.")
        print("Add changelog.d/<type>-<slug>.md and retry.")
        return 1

    print("Changelog fragment(s): " + ", ".join(fragments))
    return 0


if __name__ == "__main__":
    sys.exit(main())
