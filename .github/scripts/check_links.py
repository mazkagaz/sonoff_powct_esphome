#!/usr/bin/env python3
"""Check relative Markdown links in the repository."""

from __future__ import annotations

import os
import re
import sys
import urllib.parse

LINK = re.compile(r"!?\[[^\]]*\]\(([^)]+)\)")
SKIP_DIRS = {".git", ".esphome", "build", "node_modules"}


def main() -> int:
    root = os.getcwd()
    missing: list[tuple[str, str, str]] = []
    checked = 0

    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for filename in filenames:
            if not filename.lower().endswith(".md"):
                continue
            path = os.path.join(dirpath, filename)
            with open(path, encoding="utf-8") as handle:
                text = handle.read()
            for match in LINK.finditer(text):
                target = match.group(1).strip()
                if target.startswith("<") and target.endswith(">"):
                    target = target[1:-1]
                if not target or target.startswith("#"):
                    continue
                if re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*:", target):
                    continue
                target = urllib.parse.unquote(target.split("#", 1)[0])
                target = target.replace("\\_", "_")
                if not target:
                    continue
                resolved = os.path.normpath(
                    os.path.join(os.path.dirname(path), target)
                )
                checked += 1
                if not os.path.exists(resolved):
                    missing.append((os.path.relpath(path, root), match.group(0), resolved))

    print(f"Checked {checked} relative Markdown links.")
    if missing:
        print("Missing links:")
        for source, raw, resolved in missing:
            print(f"  {source}: {raw} -> {resolved}")
        return 1
    print("All relative links resolve.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
