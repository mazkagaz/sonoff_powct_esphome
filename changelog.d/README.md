# Changelog fragments

Add one fragment per pull request:

```
changelog.d/<type>-<short-slug>.md
```

Allowed types: `added`, `changed`, `fixed`, `removed`, `security`.

Example:

```
changelog.d/fixed-uart-timeout.md

Keep the last valid measurement when a UART read times out.
```

At release time, fragments are assembled into `../CHANGELOG.md`. This avoids
merge conflicts and keeps the changelog user-facing.
