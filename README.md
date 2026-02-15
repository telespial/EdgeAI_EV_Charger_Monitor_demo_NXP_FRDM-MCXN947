# EdgeAI EV Charger Monitor Demo (FRDM-MCXN947)

This repository is a standalone firmware project scaffold for an EV charger monitor demo on NXP FRDM-MCXN947.

Target hardware:
- FRDM-MCXN947
- LCD-PAR-S035 display (optional UI)
- EV charger input sensing hardware (project-specific integration pending)

## Project Status
- 12-minute, 20 Hz EV charging profiles are in place (`normal`, `wear`, `fault`).
- Runtime includes elapsed charge clock + connector-wear anomaly scoring for demo AI behavior.
- Cockpit LCD dashboard is implemented and hardware flash-validated.
- Golden restore point is established:
  - `GOLDEN_20260214_203015`
  - `GOLDEN_LOCK_20260214_203015_22b87ce`
  - failsafe binary in `failsafe/`.

## Quickstart
```bash
./tools/bootstrap_ubuntu_user.sh
./tools/setup_mcuxsdk_ws.sh
./tools/build_frdmmcxn947.sh debug
./tools/flash_frdmmcxn947.sh
```

## Repo Layout
- `src/`: project firmware sources
- `sdk_example/`: MCUX SDK examples overlay
- `tools/`: setup/build/flash scripts
- `docs/`: project state, runbook, restore points, and logs

Style references:
- `docs/GAUGE_STYLE.md`
- `src/gauge_style.h`

Test-data references:
- `docs/TEST_DATA_PIPELINE.md`
- `src/power_data_source.h`

Real capture references:
- `docs/CAPTURE_MODE.md`
- `tools/capture_energy_trace.sh`
- `tools/capture_uart_telemetry.sh`
- `tools/trace_convert.py`
