# Project State

Last updated: 2026-02-14
Project: `EdgeAI_EV_Charger_Monitor_demo_NXP_FRDM-MCXN947`

## Current Baseline
- Release baseline: touchscreen-debug checkpoint
- Golden tag: `GOLDEN_20260215_053739`
- Lock tag: `GOLDEN_LOCK_20260215_053739`
- Failsafe image: `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260215_053739.bin`
- Lifecycle: active integration
- Initial commit: `ee3119e`

## Firmware Behavior (Current)
- Baseline app initializes board pins/clocks/debug console.
- UART startup marker prints on boot.
- Gauge style preset `cockpit_dark_v1` is defined (`src/gauge_style.*`) and logged at boot.
- Replay test samples feed the runtime data path via `src/power_data_source.*`.
- Cockpit dashboard frame now renders on LCD using `src/gauge_render.c`.
- EV charger monitor sensing/state classifier and alarm logic are still pending.

## Next Milestones
- Add EV charger signal acquisition path.
- Implement charger-state classifier and threshold model.
- Add display UI and warning states.
- Validate on hardware and publish first golden restore point.

## Known Blockers (2026-02-15)
- AI touchscreen pill on/off control is still unreliable on target hardware.
- GT911 coordinate mapping and/or touch INT event handling needs targeted debug capture and calibration.
- This checkpoint is intentionally saved as golden/failsafe despite that blocker.

## Update 2026-02-14
- Created standalone repository scaffold with docs, scripts, source tree, and MCUX overlay.
- Connected remote `origin` to GitHub and pushed `main`.

## Update 2026-02-14
- Added a sports-cockpit gauge style baseline based on the requested visual direction.
- Added `src/gauge_style.h` and `src/gauge_style.c` and wired the module into startup logging.
- Added style documentation in `docs/GAUGE_STYLE.md` and updated `docs/TODO.md`.

## Update 2026-02-14
- Added editable replay trace input at `data/replay_trace.csv`.
- Added converter `tools/trace_csv_to_header.py` generating `src/replay_trace_generated.h`.
- Added runtime data abstraction in `src/power_data_source.h` and `src/power_data_source.c`.
- Wired sample output logging in `src/edgeai_ev_charger_monitor_demo.c`.
- Added pipeline documentation in `docs/TEST_DATA_PIPELINE.md`.

## Update 2026-02-15
- Added first cockpit-style LCD renderer (`src/gauge_render.c`) with rings, needles, and numeric fields.
- Imported display/text primitives (`src/par_lcd_s035.*`, `src/text5x7.*`) and expanded glyph coverage.
- Updated demo startup loop to render live replay samples to LCD.
- Build succeeded and flash completed via:
  - workspace: `./mcuxsdk_ws` (project-local override pattern)
  - build dir: `build_ev_charger_try3`

## Update 2026-02-15
- Fixed a render cadence bug that could appear as a blank screen.
- `src/edgeai_ev_charger_monitor_demo.c` now draws the gauge frame every loop iteration.
- Rebuilt and reflashed `build_ev_charger_try3` after the fix.

## Update 2026-02-15
- Fixed LCD bring-up initialization path:
  - switched to `BOARD_InitHardware()` in `src/edgeai_ev_charger_monitor_demo.c`
  - included `app.h` for board init declaration
- Fixed UART formatting in `src/gauge_style.c` (`\r\n` handling).
- Rebuilt and reflashed `build_ev_charger_try3`.
- Verified UART boot markers include:
  - `Gauge render: ready`
  - `Power data source: replay`

## Update 2026-02-15
- Reduced visible raster flashing in gauge UI:
  - `src/gauge_render.c` now keeps static dashboard background persistent
  - only dynamic gauge/value regions are cleared and redrawn per update
  - full-screen clear per frame removed
- `src/edgeai_ev_charger_monitor_demo.c` now redraws only when sample values change.
- Rebuilt and reflashed `build_ev_charger_try3` with anti-flicker update.

## Update 2026-02-15
- Fixed gauge/value text overlap by moving numeric readouts away from ring geometry.
- Switched dynamic rendering to incremental updates:
  - erase/redraw needle only when index changes
  - clear/redraw value bands only when value changes
- Slowed replay stepping (`POWER_REPLAY_TICK_DIVIDER`) to reduce gauge thrash.
- Rebuilt and reflashed `build_ev_charger_try3`.

## Update 2026-02-15
- Added capture-mode tooling for real board energy traces:
  - `tools/capture_energy_trace.sh` (energy CSV + UART markers)
  - `tools/trace_convert.py` (energy CSV -> `data/replay_trace.csv`)
  - `docs/CAPTURE_MODE.md` workflow
- Environment check: `mcux_energy_capture` binary is not installed on this machine.
- Capture script supports external command template via `ENERGY_CAPTURE_CMD`.

## Update 2026-02-15
- Added synthetic 5-minute EV charging dataset:
  - `data/ev_charging_demo_5min.csv` (300 samples, 1 Hz)
  - includes voltage/current/power/SOC/thermal/insulation/pilot/frequency/ripple metrics
  - includes labeled anomalies: `CURRENT_SPIKE`, `VOLTAGE_SAG`, `THERMAL_RISE`, `INSULATION_DROP`, `PILOT_NOISE`

## Update 2026-02-15
- Overwrote `data/ev_charging_demo_5min.csv` with 20 Hz telemetry:
  - 6000 rows over 300 seconds (`timestamp_s` from `0.000` to `299.950`)
  - same metrics and anomaly labels retained

## Update 2026-02-15
- Redesigned the LCD dashboard to a more professional cockpit cluster style:
  - large central analog-style gauge
  - left/right secondary gauges with compact labels
  - darker multi-layer bezel treatment to match reference aesthetic
- Added upper-right scope panel in `src/gauge_render.c` with:
  - white timeline trace
  - red timeline trace
  - yellow signal trace and `20HZ TRACE` label
- Added missing uppercase `Z` glyph in `src/text5x7.c` for dashboard labeling.
- Rebuilt and flashed `build_ev_charger_try3`; flash completed successfully on probe `#1`.

## Update 2026-02-15
- Reworked the LCD UI into three vertical quadrants:
  - Quadrant 1: primary voltage/SOC gauge
  - Quadrant 2: current and power gauges
  - Quadrant 3 (top): oscilloscope panel
  - Quadrant 3 (bottom): text terminal status panel
- Added terminal telemetry text in lower-right panel: CPU %, AI state, mode, temperature, SOC, current, and power.
- Rebuilt and flashed successfully to FRDM-MCXN947 using LinkServer probe `#1`.

## Update 2026-02-15
- Updated oscilloscope styling in `src/gauge_render.c`:
  - traces now render as white + red + green
  - bottom scope title text now renders in yellow as `NXP EDGE AI`
- Added `X` glyph support in `src/text5x7.c` for scope title rendering.
- Rebuilt and flashed successfully to FRDM-MCXN947 (LinkServer probe `#1`).

## Update 2026-02-15
- Re-themed gauge cluster to match the provided red/black performance cockpit direction:
  - dominant centered red main dial with white scale and redline accents
  - white-trimmed side gauge pods with black interiors
  - updated black/red frame accents for scope and terminal panes
- Updated base palette in `src/gauge_style.c` for red/black cluster color balance.
- Rebuilt and flashed successfully to FRDM-MCXN947 via LinkServer probe `#1`.

## Update 2026-02-15
- Updated dashboard text branding/layout in `src/gauge_render.c`:
  - removed large center `EV AI` text
  - added large yellow `NXP EDGEAI` text at bottom
  - replaced top header text with smaller `(c)RICHARD HABERKERN`
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Moved the small power gauge from the third segment into the first segment, stacked with the other small gauge.
- Updated gauge center constants in `src/gauge_render.c`:
  - `MID_TOP` at left-top region
  - `MID_BOT` at left-bottom region
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Centered bottom `NXP EDGEAI` label across the full LCD panel width instead of left segment bias.
- `src/gauge_render.c` now computes text x-position using `edgeai_text5x7_width()` and panel center constants.
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Added a charging battery level indicator above the large center gauge.
- Implemented in `src/gauge_render.c`:
  - `DrawBatteryIndicatorFrame()` for static battery outline/label
  - `DrawBatteryIndicatorDynamic()` to fill level from `soc_pct` and show percentage
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Corrected gauge hash-mark geometry to true circular arcs.
- Updated LUT paths in `src/gauge_render.c` for:
  - `DrawGaugeTicks11`, `DrawGaugeTicks13`
  - `DrawNeedle11`, `DrawNeedle13`
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Moved gauge hashmark dots inward so they render inside the circular rings instead of on the ring edge.
- Updated tick redraw radii in `src/gauge_render.c` for static and dynamic tick redraw paths.
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Removed `SPEED` label from the main gauge.
- Removed SOC data bar/line and SOC percentage text from the main gauge area.
- SOC is still represented by the battery indicator above the main gauge.
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Removed the three red hash/redline strokes from the large gauge.
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Adjusted dashboard layout per visual spacing request:
  - centered the large gauge numeric value using text-width centering
  - moved `POWER` label below the lower small gauge
  - increased vertical spacing between small gauges and reduced small-gauge radius
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Refined gauge typography/layout:
  - large gauge numeric value is now centered via dynamic text-width centering
  - `POWER` label moved below the lower small gauge
  - increased spacing between the two small gauges and reduced their radius for cleaner separation
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Shortened lower-left guide lines so they no longer overlay the `POWER` text or intrude into the lower gauge area.
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Moved upper gauge `CURRENT` label downward to match the lower `POWER` label offset style.
- Added matching short horizontal guide line near the upper label for visual symmetry.
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Further adjusted upper-left label/guide placement:
  - moved `CURRENT` further below upper gauge
  - moved matching short upper horizontal guide line lower to align with new label position
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Finalized upper-left label cleanup:
  - moved `CURRENT` text upward/downward balance to clear gauge overlap
  - aligned the short upper horizontal guide to avoid text collisions
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Shifted left vertical guide line further left to increase separation from the small gauges.
- Realigned `CURRENT` text to match line relationship used by `POWER` (text offset relative to short horizontal guide).
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Added large in-gauge unit letters for readability:
  - `A` inside upper small gauge (amps)
  - `W` inside lower small gauge (watts)
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Added a full-height green accent bar at the extreme left side of the dashboard region.
- Bar spans top-to-bottom section and terminates at the white vertical guide line.
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-15
- Updated left-side guide/text alignment:
  - moved left guide structure farther left for more spacing from gauges
  - aligned `CURRENT` text offset with its short horizontal guide and with `POWER` label pattern
- Rebuilt and flashed successfully to FRDM-MCXN947.

## Update 2026-02-14
- Shortened both small-gauge pointers to match the reduced needle style used on the main gauge.
- Updated `src/gauge_render.c` small-gauge needle radii from `MID_R - 16` to `MID_R - 20` in erase/redraw paths for consistent rendering.
- Rebuilt and flashed successfully to FRDM-MCXN947 (LinkServer probe `#1`).

## Update 2026-02-14
- Changed the large gauge center fill to yellow as the base/back layer.
- Updated `src/gauge_render.c`:
  - main inner ring fill color set to `RGB565(255, 208, 52)`
  - main dynamic value-band clear color set to `RGB565(255, 208, 52)` to keep the center consistently yellow
- Rebuilt and flashed successfully to FRDM-MCXN947 (LinkServer probe `#1`).

## Update 2026-02-14
- Darkened the large gauge center fill to a very dark yellow for a deeper cockpit look.
- Updated `src/gauge_render.c` main center fill and center value-band clear color to `RGB565(120, 95, 18)`.
- Rebuilt and flashed successfully to FRDM-MCXN947 (LinkServer probe `#1`).

## Update 2026-02-14
- Darkened the large gauge center fill again to an even deeper yellow tone.
- Updated `src/gauge_render.c` center-base and dynamic center clear color to `RGB565(78, 60, 10)`.
- Rebuilt and flashed successfully to FRDM-MCXN947 (LinkServer probe `#1`).

## Update 2026-02-14
- Replaced the left vertical green accent box with a segmented vertical bargraph.
- Added `DrawLeftBargraphFrame()` and `DrawLeftBargraphDynamic()` in `src/gauge_render.c`.
- Bargraph now tracks live current (`current_mA`) with 20 segments and threshold colors (green/yellow/red).
- Rebuilt and flashed successfully to FRDM-MCXN947 (LinkServer probe `#1`).

## Update 2026-02-14
- Rewired the left vertical bargraph to temperature instead of current.
- Added over-temperature behavior in `src/gauge_render.c`:
  - bargraph turns red when `temp_c >= 70`
  - bottom label below the lower horizontal line now shows `TEMP: XXC`
- Added dynamic bottom temperature text update under the bargraph region.
- Rebuilt and flashed successfully to FRDM-MCXN947 (LinkServer probe `#1`).

## Update 2026-02-14
- Added real telemetry backend in `src/real_telemetry.c` + `src/real_telemetry.h`:
  - initializes OPAMP0 and LPADC (ADC0 command/trigger path)
  - performs startup zero-offset calibration
  - computes current/power from measured ADC delta and publishes `power_sample_t`
- Extended `power_data_source` with `POWER_DATA_SOURCE_REAL_TELEMETRY` mode:
  - selected automatically when telemetry init succeeds
  - replay mode remains fallback if real telemetry is unavailable
- Updated main loop to 20 Hz sampling cadence and structured UART CSV output:
  - `TELEM_HEADER,...`
  - `TELEM_CSV,t_ms,current_mA,power_mW,voltage_mV,temp_c,soc_pct,mode`
- Added host capture utility `tools/capture_uart_telemetry.sh` for one-command UART CSV capture.
- Build succeeded in workspace `build_ev_charger_try3`.
- Flash attempt failed with LinkServer target-connect error (`Cannot find MEM-AP ... check target power`), so new image is built but not yet confirmed flashed on hardware.

## Update 2026-02-15
- Added synthetic EV charge profile generation tooling:
  - `tools/generate_ev_charge_profiles.py`
  - outputs 12-minute, 20 Hz traces with `1 min real = 1 hour simulated` charge progression.
- Generated profile assets:
  - `data/ev_charge_12min_normal_20hz.csv`
  - `data/ev_charge_12min_wear_20hz.csv`
  - `data/ev_charge_12min_fault_20hz.csv`
- Set firmware replay source to wear profile via regenerated:
  - `data/replay_trace.csv`
  - `src/replay_trace_generated.h` (14400 samples).
- Extended sample/runtime model with:
  - `elapsed_charge_s`, `elapsed_charge_sim_s`
  - `connector_wear_pct`
  - `anomaly_score_pct`
- Updated dashboard rendering (`src/gauge_render.c`):
  - elapsed clock displayed inside center gauge bottom area
  - elapsed block centered in second section
  - oscilloscope traces include random noise jitter for realism
  - center gauge yellow tone tuned per latest style requests
- Updated main loop (`src/edgeai_ev_charger_monitor_demo.c`) to explicit 20 Hz cadence.
- Rebuilt and flashed successfully after each layout/style refinement using LinkServer probe `#1`.

## Update 2026-02-15 (Golden + Failsafe)
- Created golden restore point tags:
  - `GOLDEN_20260214_203015`
  - `GOLDEN_LOCK_20260214_203015_22b87ce`
- Created failsafe restore artifact package:
  - `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260214_203015.bin`
  - `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260214_203015.sha256`
  - `failsafe/GOLDEN_20260214_203015_metadata.txt`
- Updated restore documentation:
  - `docs/START_HERE.md`
  - `docs/RESTORE_POINTS.md`
  - `docs/failsafe.md`

## Update 2026-02-15
- Documented unresolved touchscreen AI-pill issue as an active blocker.
- Updated normal AI status behavior to render solid green `SYSTEM STABLE` and reserve flashing behavior for warning/fault states only.
- Created new golden/failsafe restore package:
  - `GOLDEN_20260215_053739`
  - `GOLDEN_LOCK_20260215_053739`
  - `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260215_053739.bin`

## Update 2026-02-15
- Build attempt via `./tools/build_frdmmcxn947.sh debug` failed during CMake configure.
- Error: `mcux_set_variable` incorrect arguments from `mcuxsdk/middleware/edgefast_bluetooth/CMakeLists.txt`.

## Update 2026-02-15
- Stabilized workspace/build integration for this project:
  - `tools/build_frdmmcxn947.sh` now reinstalls the project overlay into workspace examples on every build.
  - `sdk_example/install_mcux_overlay.sh` now replaces existing demo overlay folders to prevent stale placeholder files.
  - `tools/patch_mcuxsdk.sh` now guards `mcuxsdk/middleware/eiq/CMakeLists.txt` so missing `middleware/eiq/mpp/CMakeLists.txt` is skipped instead of failing configure.
- AI toggle debug + behavior changes in `src/edgeai_ev_charger_monitor_demo.c`:
  - removed false-positive toggle path that treated any GT911 INT falling edge as a valid AI pill tap
  - toggling now happens only on validated new press events that map into AI-pill hitbox
  - added UART debug markers:
    - `TOUCH_PRESS,x=...,y=...,hit=...,map_var=...,mx=...,my=...`
    - `TOUCH_HOLD,x=...,y=...,hit=...`
    - `TOUCH_INT_EDGE,without_point`
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` now completes successfully and produces:
    - `mcuxsdk_ws/build/edgeai_ev_charger_monitor_demo_cm33_core0.bin`

## Update 2026-02-15
- Flashed latest built firmware to hardware:
  - command: `./tools/flash_frdmmcxn947.sh`
  - runner: LinkServer v25.12.83
  - probe: `#1` (`2PZWMSBKUXU22`)
  - target: `MCXN947:FRDM-MCXN947`

## Update 2026-02-15
- Applied second-pass touchscreen fix for AI pill:
  - `src/edgeai_ev_charger_monitor_demo.c`
  - GT911 INT pin mode callback now actively drives GPIO high/low for init/address sequencing (instead of only pull resistors).
  - GT911 reset callback now drives reset GPIO (`PIO4_7`) so reset sequencing is no longer a no-op.
  - AI hit testing now uses raw GT911 coordinates scaled to LCD space with rotate/mirror variants.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`)

## Update 2026-02-15
- Added third-pass touch fallback behavior in `src/edgeai_ev_charger_monitor_demo.c`:
  - if GT911 INT falls but no coordinate packet is available in that loop, firmware now treats it as a controlled tap fallback for AI toggle
  - toggle source is logged as `src=touch_int_edge`
  - coordinate-based press toggle path remains active as preferred path
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`)

## Update 2026-02-15
- Replaced EV demo touch-toggle flow with the same model used in:
  - `/home/user/python_projects/codemaster/projects/nxp/frdm-mcxn947/EdgeAI_3D_Printer_Spool_Size_ToF_demo_NXP_FRDM-MCXN947/src/tof_demo.c`
- Ported behavior:
  - single GT911-to-LCD transform (`x = selected->y`, `y = resolutionX - selected->x`)
  - touch poll cadence gate (`TOUCH_POLL_TICKS`) instead of per-loop edge heuristics
  - toggle only on touch down edge (`pressed && !was_down`) while inside AI pill hitbox
  - removed INT-edge fallback auto-toggle path that could random-trigger
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`)

## Update 2026-02-15
- Implemented option 2 (touch-priority rendering mode):
  - `src/edgeai_ev_charger_monitor_demo.c`
  - `TOUCH_PRIORITY_UI_MODE` enabled
  - dashboard is no longer continuously refreshed on sample changes
  - touch button polling/toggle path remains active; UI redraw occurs on AI toggle events
- Intended effect:
  - remove display redraw load as a source of touch intermittency
  - isolate button behavior while keeping gauges visible
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`)

## Update 2026-02-15
- Updated AI touchbutton geometry/placement:
  - file: `src/gauge_render.h`
  - moved to centered top of screen
  - increased button size for easier touch interaction
  - new bounds:
    - `GAUGE_RENDER_AI_PILL_X0 142`
    - `GAUGE_RENDER_AI_PILL_Y0 4`
    - `GAUGE_RENDER_AI_PILL_X1 337`
    - `GAUGE_RENDER_AI_PILL_Y1 39`
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`)

## Update 2026-02-15
- Replaced the main app with a minimal touch validation program in `src/edgeai_ev_charger_monitor_demo.c`.
- Runtime behavior is now intentionally simplified for debug:
  - black screen
  - one centered rectangular touch button only (`AI ON` / `AI OFF`)
  - no gauges, no telemetry widgets, no continuous dashboard redraw
  - touch is polled in the main loop and toggle occurs on touch-down edge inside the button
- GT911 handling retained in this test:
  - INT and RST GPIO callbacks
  - same coordinate transform path used in recent touch fixes (`x = y`, `y = resolutionX - x`)
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`)

## Update 2026-02-15
- Reflashed the current touch-only validation firmware without source changes for another on-hardware test cycle.
- Validation:
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`)

## Update 2026-02-15
- Touch-read debug hardening for the minimal touch test app in `src/edgeai_ev_charger_monitor_demo.c`:
  - aligned GT911 INT pin callback behavior with known-good 3D-printer demo:
    - added `CLOCK_EnableClock(kCLOCK_Port4)`
    - use passive pull config only in `TouchConfigIntPin()` (no GPIO output drive in callback)
  - reset callback changed to no-op to match known-good behavior (`TouchConfigResetPin()`).
  - touch-down diagnostics now print on every new press edge:
    - `TOUCH_DOWN,<x>,<y>,IN_BUTTON|OUTSIDE_BUTTON`
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Further aligned minimal touch test app to known-good ToF initialization sequence:
  - removed explicit `LPI2C_MasterInit()`/clock-source setup in `TouchInit()`
  - removed manual pre-init of touch INT/RST pins from `main()`
  - kept GT911 init callback set and added explicit `GT911_Init` status logging
- Goal:
  - eliminate app-specific I2C/pin setup differences that may block touch packets on this board/LCD stack.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Adjusted startup order in `src/edgeai_ev_charger_monitor_demo.c`:
  - LCD init + initial button draw now run before `TouchInit()`.
- Purpose:
  - avoid blank-screen symptom if GT911 init path blocks/fails during startup.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Applied LCD module MRD guidance to touch test firmware:
  - removed remaining `P4_7` touch-reset assumptions/constants from `src/edgeai_ev_charger_monitor_demo.c`
  - removed direct GPIO dependency for touch-reset control
  - GT911 reset callback remains explicit no-op with note that `P4_7` is LCD reset ownership (`LCD_RST`)
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Added hard diagnostic mode in `src/edgeai_ev_charger_monitor_demo.c`:
  - AI state now toggles on any touch-down edge (no button hitbox requirement)
  - UART log marks source as `src=any_touch`
  - coordinate logging remains enabled (`TOUCH_DOWN,...`)
- Purpose:
  - determine whether any GT911 touch packets are being received at all, independent of mapping/hitbox logic.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Ported a critical board-init dependency from the working ToF project:
  - file: `sdk_example/mcuxsdk_examples_overlay/_boards/frdmmcxn947/demo_apps/edgeai_ev_charger_monitor_demo/cm33_core0/hardware_init.c`
  - added Arduino I2C clock setup for FLEXCOMM2:
    - `CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u);`
    - `CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);`
- Rationale:
  - GT911 touch path uses `LPI2C2` (FC2); EV app `cm33_core0` init previously configured FC3 but not FC2.
  - This mismatch could prevent touch controller transactions regardless of app-level touch logic.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Ported another critical ToF dependency into `src/edgeai_ev_charger_monitor_demo.c`:
  - explicit FC2/LPI2C2 master initialization before GT911 init:
    - `CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u)`
    - `CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2)`
    - `LPI2C_MasterInit(LPI2C2, ..., 400000u)`
- Rationale:
  - ToF app has FC2 I2C master initialized by `tmf8828_quick_init()` before touch init.
  - This EV touch-only app had no equivalent peripheral init path, so GT911 I2C transfers could fail even with correct touch logic.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Removed temporary touch diagnostics and restored intended UI behavior:
  - in `src/edgeai_ev_charger_monitor_demo.c`, AI toggle now triggers only when press-edge is inside the rendered button bounds.
  - removed `src=any_touch` diagnostic toggle behavior.
- Retained all proven touch-path fixes:
  - FC2 clock setup in `cm33_core0/hardware_init.c`
  - explicit `LPI2C2` master init before `GT911_Init`
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Reintegrated touch into the full EV charger dashboard application in `src/edgeai_ev_charger_monitor_demo.c`:
  - restored dashboard runtime loop using:
    - `PowerData_Init() / PowerData_Tick() / PowerData_Get()`
    - `GaugeRender_Init() / GaugeRender_DrawFrame()`
  - AI touchbutton now uses the dashboard pill bounds from `src/gauge_render.h`:
    - `GAUGE_RENDER_AI_PILL_X0..Y1`
  - touch polling and toggle behavior:
    - poll GT911 every 20 ms
    - toggle only on press-edge inside AI pill bounds
    - redraw dashboard immediately on toggle
  - retained the proven touch fixes:
    - FC2 clock path in `cm33_core0/hardware_init.c`
    - explicit `LPI2C2` init before `GT911_Init()`
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Updated AI pill geometry in dashboard renderer:
  - file: `src/gauge_render.h`
  - width changed to half of previous size:
    - from `X0=142, X1=337` (196 px) to `X0=191, X1=288` (98 px)
  - height changed to 3/4 of previous size:
    - from `Y0=4, Y1=39` (36 px) to `Y0=4, Y1=30` (27 px)
- Updated label placement in `src/gauge_render.c`:
  - text Y is now vertically centered using pill height (`ly` calculation) so `AI ON/OFF` fits cleanly in the smaller box.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Implemented dashboard typography improvement without adding heavy new font assets:
  - file: `src/gauge_render.c`
  - added `DrawTextUi()` helper that renders text with a 1px black drop-shadow.
  - replaced direct `edgeai_text5x7_draw_scaled()` calls for major dashboard text with `DrawTextUi()`:
    - AI pill label
    - scope/terminal headers
    - status/alert text
    - terminal telemetry lines
    - gauge labels/brand text/value overlays
- Effect:
  - improved legibility and perceived font quality over busy backgrounds while keeping existing MCU/LCD performance profile.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Decoupled data processing cadence from display redraw cadence in `src/edgeai_ev_charger_monitor_demo.c`:
  - `PowerData_Tick()` remains at 20 Hz (`POWER_TICK_PERIOD_US = 50000`)
  - LCD rendering is now throttled to 500 ms (`DISPLAY_REFRESH_PERIOD_US = 500000`)
  - AI touch toggle still forces an immediate redraw for responsive UI feedback
- Effect:
  - reduced visible raster/wipe sensation while preserving internal sample/update fidelity.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Implemented stricter dirty-region rendering in `src/gauge_render.c` to reduce visible raster wipe:
  - Scope panel:
    - replaced full-rectangle clear/redraw with incremental column updates
    - clears only current scope column and redraws only the new trace segments
    - keeps baseline and prior trace history persistent between updates
  - Terminal panel:
    - replaced full terminal content clear with per-line dirty clears (`DrawTerminalLine`)
    - each status line now clears/redraws only its own text band
- Result:
  - lower visible wipe/flash during refresh while preserving existing UI layout and data fidelity.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Addressed minor dashboard polish issues in `src/gauge_render.c` and `src/text5x7.c`:
  - main gauge center restore:
    - added `RefreshMainGaugeCenter()` and invoke it whenever the large needle index changes
    - prevents black trail/holes in yellow center fill when old needle is erased
  - numeric spacing cleanup:
    - removed field-width padding that inserted leading spaces in live values (`V`, `A`, `W`)
    - updated both center gauge values and terminal `CUR/PWR` lines
  - normal alert behavior:
    - replaced `"SYSTEM STABLE"` with `"SYSTEM NORMAL"` in the normal-state green box
    - warning-without-fault condition now renders as normal green status instead of yellow warning
  - missing glyph fix:
    - added uppercase `Y` glyph in `src/text5x7.c` so `SYSTEM NORMAL` renders correctly.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Added AI-off status logic and decimal rendering fix:
  - `src/gauge_render.c`
    - introduced rule-based status evaluation for AI-off mode using live thresholds:
      - `FAULT`: `temp >= 72C` or `voltage < 22300mV` or `current >= 38000mA`
      - `WARNING`: `temp >= 66C` or `voltage < 22800mV` or `current >= 34000mA`
      - else `NORMAL`
    - alert overlay now continues to render status when AI is OFF using the above rule path
    - terminal `SYS` status now reflects rule status when AI is OFF (instead of `RAW`)
  - `src/text5x7.c`
    - added `.` glyph mapping to fix decimal value rendering (`37.3` no longer appears as `37  3`)
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Reworked replay data generation for realistic AI-analysis input at 20 Hz:
  - file: `src/power_data_source.c`
  - removed dependency on static generated replay table for runtime sampling
  - added procedural EV charging cycle model:
    - duration: ~11 simulated hours per cycle (`REPLAY_CYCLE_HOURS=11`)
    - cadence: 20 Hz (`POWER_SAMPLE_PERIOD_MS=50`)
    - profile phases: ramp-up, bulk high current, taper, CV taper, top-off
    - includes high thermal peak region up to ~82C (180F) during peak current
    - deterministic high-rate ripple/noise added to current/voltage/temp/power to produce realistic telemetry variation for model analysis
- Updated scope behavior for requested 6-hour trend window:
  - file: `src/gauge_render.c`
  - scope header text changed to `6H SNAPSHOT`
  - scope trace sampling now buckets by simulated time (`21600s / 96 points = 225s per point`)
  - trace advances only when a new bucket is reached, so on-screen trend represents approximately 6 simulated hours.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Applied requested scope and threshold tuning:
  - file: `src/gauge_render.c`
  - scope now plots only:
    - power (white trace), scaled to `14kW` full-scale
    - temperature (orange trace), scaled to `100C` full-scale
  - removed voltage/current traces from scope rendering path
  - scope header updated to `6H PWR TEMP`
- Updated AI-off safety logic to use requested temperature/power limits:
  - `WARNING` at:
    - `temp >= 75C` (near overtemp)
    - or `power >= 10kW` (overcurrent warning)
  - `FAULT` at:
    - `temp >= 85C` (shutdown temperature warning state)
    - or `power >= 12kW` (overcurrent)
  - reason strings now map to:
    - `NEAR OVERTEMP`
    - `SHUTDOWN TEMP`
    - `OVERCURRENT WARN`
    - `OVERCURRENT`
- Replay model power headroom adjusted so thresholds are exercisable:
  - file: `src/power_data_source.c`
  - raised high-current phase peak and increased output clamp to `14kW`.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Scope motion/grid refinement:
  - file: `src/gauge_render.c`
  - scope trace now advances right-to-left (new samples originate from the right and move leftward over time).
  - added persistent L-shaped axis/grid lines in scope area:
    - left vertical axis
    - bottom horizontal axis
  - axis pixels are redrawn during incremental column clear so the L-shape remains stable during updates.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Scope follow-up refinement:
  - file: `src/gauge_render.c`
  - restored scope motion to left-to-right per latest UX request.
  - removed tiny vertical artifact above terminal `STATUS` by tightening L-axis endpoints to stay inside scope plot bounds (`ph - 1`).
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Display follow-up fixes:
  - file: `src/gauge_render.c`
  - terminal header:
    - added targeted cleanup fill to remove persistent tiny `|` artifact above `STATUS`.
  - center gauge:
    - restored explicit `VAC` label in main gauge section.
  - lower (power) mini-gauge value:
    - changed from full integer watts to centered decimal `kW` rendering (`X.Y`) so value fits gauge window and no longer appears blank/clipped at high power.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Layering/artifact follow-up:
  - file: `src/gauge_render.c`
  - main center voltage text:
    - added explicit redraw trigger when main-gauge face is refreshed during needle movement, ensuring text remains top-layer over yellow center fill.
  - terminal header artifact:
    - added header-band refresh/repaint each terminal dynamic update to remove persistent `|` above `STATUS`.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Strict artifact isolation pass:
  - file: `src/gauge_render.c`
  - disabled scope axis (`L` grid) drawing entirely in `DrawScopeDynamic` with no other functional changes.
  - purpose: isolate whether persistent `|` above `STATUS` originates from scope axis rendering path.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Post-isolation corrective pass:
  - file: `src/gauge_render.c`
  - re-enabled scope L-grid drawing with bounded left/bottom axis coordinates.
  - added explicit fill clear for the inter-panel gap (`scope bottom + 1` to `terminal top - 1`) so stray pixels cannot persist above `STATUS`.
  - fixed dynamic value redraw dependency:
    - when value bands are cleared, center voltage/current/power text is forced to redraw even if the numeric value did not change this frame.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Voltage text layout request applied:
  - file: `src/gauge_render.c`
  - main center voltage now renders as a single centered string with suffix:
    - format: `NNNVAC`
  - moved voltage string down by one text height (scale 3) to requested lower position.
  - removed previous separate static `VAC` label to prevent overlap/double-label.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Replay dataset realism upgrade for AI analysis:
  - file: `src/power_data_source.c`
  - cycle duration updated to 12 hours (`REPLAY_CYCLE_HOURS=12`) at 20 Hz sample cadence.
  - profile updated to realistic staged charging behavior:
    - precharge/ramp
    - bulk high-current
    - incident window
    - derate/recovery
    - CV taper
    - top-off
  - deterministic incident windows injected:
    - overcurrent burst
    - voltage sag under load
    - overvoltage rebound pulse
    - thermal runaway / excessive heat period
    - repeating terminal-stress transients
  - adjusted operational clamps to support anomaly windows while staying within dashboard plot scales.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Added interactive timeline scrub bar above scope:
  - files:
    - `src/gauge_render.h`
    - `src/gauge_render.c`
    - `src/edgeai_ev_charger_monitor_demo.c`
  - removed `6H PWR TEMP` scope title and replaced with horizontal touch scrub bar.
  - implemented history-backed scope rendering with windowed navigation:
    - keeps 12h trace history (power + temp samples)
    - displays 6h window (96 points)
    - dragging scrub bar pans the displayed window across available history.
  - touch wiring:
    - new `GaugeRender_HandleTouch(x,y,pressed)` API
    - main loop now triggers immediate redraw while scrub position changes.
  - scrub behavior:
    - drag to any position to browse history
    - release near right edge resumes auto-follow of latest data.
- Build fix:
  - removed unused legacy incremental-scope symbols after the full-window scrub refactor to satisfy `-Werror`.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Timeline UX simplification per request:
  - file: `src/gauge_render.c`
  - replaced swipe/scrub interaction with two touch buttons above the graph:
    - left button `<` decrements timeline
    - right button `>` increments timeline
  - timeline now steps in discrete 1-hour increments from `0H` to `12H`.
  - center indicator shows current hour selection (`0H..12H`).
  - touch handling changed to edge-triggered button press (`touch latch`) for stable single-step behavior.
- Build hygiene:
  - removed remaining scrub-era/legacy symbols that were unused under the new button-step model.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Timeline button visibility fix:
  - file: `src/gauge_render.c`
  - controls are now drawn in the static scope frame path as well as dynamic updates, so they appear immediately.
  - enlarged left/right button widths for easier hit and visual clarity.
  - switched labels from `<` / `>` to `L` / `R` because current bitmap font does not include angle-bracket glyphs.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Timeline hour box height increased per request:
  - file: `src/gauge_render.c`
  - timeline control bar made approximately 3x taller by increasing `TIMELINE_Y1`.
  - left/right button labels and center hour text are vertically centered in the new taller box.
  - scope plot origin moved below the expanded timeline box to avoid plot overlap.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Touch response and timeline placement update:
  - files:
    - `src/edgeai_ev_charger_monitor_demo.c`
    - `src/gauge_render.c`
  - touch poll period reduced from `20 ms` to `10 ms` for quicker UI response.
  - timeline hour control bar moved to top (`y=4..30`) as requested.
  - scope plot origin restored to pre-timeline geometry so graph regains original height.
  - left/right timeline buttons now support hold-to-repeat stepping for faster navigation across `0H..12H`.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Timeline hour controls now drive dataset playback:
  - files:
    - `src/power_data_source.h`
    - `src/power_data_source.c`
    - `src/gauge_render.h`
    - `src/gauge_render.c`
    - `src/edgeai_ev_charger_monitor_demo.c`
  - added `PowerData_SetReplayHour(hour)` to lock replay to a selected 1-hour window.
  - replay advances normally inside that hour and loops to the start of that same hour when window end is reached.
  - timeline changes now call `PowerData_SetReplayHour(...)`, so gauge values and plot feed from selected-hour playback (not full-cycle free-run).
  - startup now initializes replay window from current timeline selection.
  - timeline `12H` clamps to final valid dataset hour window (`11:00-11:59`).
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Scope readability and units update:
  - file: `src/gauge_render.c`
  - corrected power text formatting to `kW`:
    - bottom mini gauge unit changed from `W` to centered `kW`
    - terminal power line changed from `PWR NNW` to `PWR N.NkW`
  - scope line colors updated:
    - power trace = orange
    - temperature trace = blue
  - added color-matched legend text (`PWR`, `TEMP`) below the scope horizontal axis.
  - scope window render source now uses latest replay-window segment directly (instead of hour-proportional historic offset) to avoid stale saturated traces.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Right-gauge `kW` and power-consistency fix:
  - files:
    - `src/text5x7.c`
    - `src/gauge_render.c`
  - text renderer now maps lowercase `k` to glyph `K`, so `"kW"` no longer renders as only `"W"`.
  - right power gauge unit text changed to smaller, centered `"kW"` for fit/readability.
  - rendered power value path is now derived from displayed electrical values:
    - `power_W = (voltage_display_units * current_mA) / 100000`
    - used consistently for right-gauge needle/value, terminal `PWR` line, and scope power trace scaling.
  - objective: prevent mismatch such as low displayed current with pegged `14.0` kW readout.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Elapsed timer replay-hour sync and layering update:
  - files:
    - `src/power_data_source.c`
    - `src/gauge_render.c`
  - elapsed time now derives from replay position (`replay_index`) in replay mode, so it tracks the selected hour window and advances/loops with that same hour.
  - elapsed redraw trigger changed from minute-only to per-second update.
  - `ELAPSED` label/value block moved lower in the main gauge region.
  - elapsed draw moved to final dynamic pass position so text stays on top and is not hidden by later panel refreshes.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Elapsed box width refinement:
  - file: `src/gauge_render.c`
  - elapsed background box now auto-sizes to text width (max of `ELAPSED` and `HH:MM:SS`) with a fixed `3 px` left/right margin.
  - removes excess box width so the yellow band interferes less with gauge visuals.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Minor gauge text position update:
  - file: `src/gauge_render.c`
  - moved center `VAC` text up by `8 px`.
  - moved elapsed label/time block up by `10 px` (including its background band).
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Header/timeline default update:
  - file: `src/gauge_render.c`
  - moved name text higher into the top header line area.
  - changed default timeline selection to `3H` (from `12H`) for initial startup.
  - aligned timeline reset path to also restore to `3H`.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Regression recovery update:
  - files:
    - `src/power_data_source.c`
    - `src/gauge_render.c`
  - replay-index to elapsed-sim conversion now reapplies `POWER_SIM_TIME_SCALE`, restoring expected scope sampling cadence and visible line progression.
  - name text moved further up in top header line (`PANEL_Y0 + 5`).
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Unit label typography and header alignment update:
  - file: `src/gauge_render.c`
  - changed top-left gauge unit `A` to same font scale as right gauge `kW` and centered the unit text.
  - moved name text further up in the top header line.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Name-to-AI top alignment update:
  - file: `src/gauge_render.c`
  - header name text Y now directly uses `AI_PILL_Y0`, aligning the top of `(c)RICHARD HABERKERN` with the top edge of the AI on/off button.
- Validation:
  - `./tools/build_frdmmcxn947.sh debug && ./tools/flash_frdmmcxn947.sh` PASS

## Update 2026-02-15
- Golden/failsafe restore point created:
  - tag base: `GOLDEN_20260215_150517`
  - failsafe artifacts:
    - `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260215_150517.bin`
    - `failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260215_150517.sha256`
    - `failsafe/GOLDEN_20260215_150517_metadata.txt`
  - restore catalog updated: `docs/RESTORE_POINTS.md`
