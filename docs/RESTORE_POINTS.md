# Restore Points

## 2026-02-14 Golden Baseline
- Golden tag: `GOLDEN_20260214_203015`
- Lock tag: `GOLDEN_LOCK_20260214_203015_22b87ce`
- Failsafe binary: `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260214_203015.bin`
- Failsafe checksum: `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260214_203015.sha256`
- Metadata: `failsafe/GOLDEN_20260214_203015_metadata.txt`

### Baseline Contents
- 12-minute replay dataset at 20 Hz (`1 minute = 1 hour simulated charge time`).
- Profiles generated: normal, wear, fault (`data/ev_charge_12min_*_20hz.csv`).
- Default firmware replay sourced from wear profile (`data/replay_trace.csv` -> `src/replay_trace_generated.h`).
- AI connector-wear anomaly scoring and terminal status readouts.
- Elapsed charge clock rendered inside center gauge.
- Oscilloscope lines include random noise jitter for realistic trace behavior.

### Restore Steps
1. Build from current baseline or use the failsafe binary directly.
2. Flash:
   `LinkServer flash --probe '#1' --update-mode none MCXN947:FRDM-MCXN947 load --addr 0x0 failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260214_203015.bin`
3. Verify UART samples and LCD dashboard behavior.
