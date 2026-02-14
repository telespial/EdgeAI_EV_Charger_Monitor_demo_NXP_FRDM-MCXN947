# Test Data Pipeline

Purpose:
- provide editable test traces for gauge bring-up before live hardware capture is integrated.

## Files
- editable CSV input: `data/replay_trace.csv`
- generated firmware trace header: `src/replay_trace_generated.h`
- converter script: `tools/trace_csv_to_header.py`
- runtime data source API: `src/power_data_source.h`, `src/power_data_source.c`

## Update Flow
1. Edit trace values in `data/replay_trace.csv`.
2. Regenerate C header:
   ```bash
   ./tools/trace_csv_to_header.py --in data/replay_trace.csv --out src/replay_trace_generated.h
   ```
3. Rebuild and flash:
   ```bash
   ./tools/build_frdmmcxn947.sh debug
   ./tools/flash_frdmmcxn947.sh
   ```

## Runtime Modes
- `replay`: steps through generated trace points.
- `live_override`: holds one injected sample value (intended for future UART/debug command injection).

`src/edgeai_ev_charger_monitor_demo.c` now calls `PowerData_Tick()` in the main loop and logs samples to UART for verification.
