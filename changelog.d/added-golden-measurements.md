Added golden measurement tests that compare the production offset pipeline with
the stock pipeline on the same raw U/I/P inputs. The baseline covers 220/240 V,
0-100 A and an export power-sign case, and is generated from code. Expected
changes require `approved_by`/`approval_reason`; drift above 2 % requires
explicit `drift_approved_by`/`drift_approval_reason` and a bounded
`drift_approved_pct`.
