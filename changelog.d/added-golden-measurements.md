Added golden measurement tests that feed raw U/I/P samples through the
**production offset pipeline** and fail on any unexpected difference. The
baseline covers 220/240 V, 0-100 A and an export power-sign case, and is
generated from the production conversion code. Reference changes require
explicit `approved_by`/`approval_reason` metadata.
