# Manual Reference Validation — Sonoff POW CT vs Shelly EM

**Status: NOT EXECUTED. This document is a procedure and a data template only.**  
**Last prepared:** 2026-11-25  
**Related:** [TEST_PLAN.md](TEST_PLAN.md), [TEST_MATRIX.md](TEST_MATRIX.md),
[SPEC.md](SPEC.md)

## 1. Purpose

Provide a reproducible, manual comparison between a real Sonoff POW CT running
this firmware and a third-party Shelly EM on the same circuit.

This is **not** a calibrated metrology test and **not** an automated HIL test. It
is a reference comparison intended to catch gain, offset, range, sign and
energy-integration errors.

## 2. Limitations

- The Shelly EM is a consumer device, not a calibrated reference meter.
- Both devices have their own CT, burden and calibration; differences are not
  attributed to the POW CT alone.
- The comparison is manual and does not exercise automated OTA, reboot or
  long-term network behaviour.
- No result may be reported as a certification or as a full HIL pass.
- The status remains **NOT EXECUTED** until the CSV is filled and a results
  report is linked here.

## 3. Safety

- Mains voltage is present. Work de-energized and have the installation
  performed or checked by a qualified person.
- Never open a CT secondary circuit while current is flowing.
- Use two CTs on the same conductor, with the same direction and the same
  circuit; do not compare different phases or different loads.
- Respect the Shelly EM and POW CT terminal ratings and local regulations.

## 4. Equipment

- One standard Sonoff POW CT with the firmware under test.
- One Shelly EM (model/revision recorded in the CSV).
- Two CT clamps: one for the POW CT, one for the Shelly EM, on the same live
  conductor and in the same direction.
- A controllable, stable load: 0, 50, 100, 500, 1000, 2000 W; 3000 W only if the
  installation is rated and safe.
- A way to read both devices simultaneously: Home Assistant, Shelly app/API and
  the POW CT logs/sensors.
- [reference_measurements_template.csv](../tests/hil/reference_measurements_template.csv).

## 5. Test cases

| ID | Case | Load / condition | Expected comparison |
|---|---|---|---|
| REF-ACC-001 | Voltage/current/power, low range | 50 W, 100 W | V/I within provisional ±1 %, P within ±2 % |
| REF-ACC-002 | Voltage/current/power, high range | 1000 W, 2000 W | V/I within provisional ±1 %, P within ±2 % |
| REF-ACC-003 | Range transition | Around 500 mA | No discontinuity or persistent offset |
| REF-ACC-004 | Anti-creep / zero | 0 W, CT installed | Both report near zero; no false energy |
| REF-ACC-005 | Energy integration | 1 h and 24 h at a stable load | Energy difference documented within provisional tolerance |
| REF-ACC-006 | Export (optional) | Reversed CT or generation source, if Shelly EM supports it | Sign and export energy consistent |

Tolerances are **provisional**. Record the Shelly EM model and its stated
accuracy; if the combined uncertainty is larger, widen the tolerance and explain
why in the results.

## 6. Procedure

1. Record date, firmware version, POW CT revision, Shelly EM model and CT
   orientation in the CSV.
2. Verify both CTs are on the same conductor and the same direction.
3. For each load:
   - set the load and wait for thermal/electrical stabilization;
   - record V, I and P from both devices over a 5-minute steady window;
   - repeat three times;
   - record the average and the raw window in `notes`.
4. For the range transition, step through approximately 400–600 mA and note any
   discontinuity.
5. For energy, start both counters from a known point, run 1 h (and 24 h if
   practical), then record both deltas.
6. If export is tested, record whether the Shelly EM reports negative power or a
   separate export channel; do not assume equivalence.
7. Copy the completed CSV to a dated results file and write a short result
   summary linked from this document.

## 7. Acceptance and reporting

A case is:

- **PASS** when the measured difference is within the agreed provisional
  tolerance and the trend is stable;
- **FAIL** when it exceeds tolerance or shows a discontinuity/sign error;
- **INCONCLUSIVE** when the load, CT placement or reference uncertainty is not
  good enough to decide.

Every result must state that this is a **manual reference comparison**, not a
calibrated accuracy claim.

## 8. Completion checklist

- [ ] CSV populated with real measurements.
- [ ] Results summary written and linked here.
- [ ] Deviations filed as issues with firmware version and load.
- [ ] `TEST_MATRIX.md` updated from **Pending** to the actual result.
- [ ] Status changed from **NOT EXECUTED** only after the above.

Until then, this validation remains **NOT EXECUTED**.
