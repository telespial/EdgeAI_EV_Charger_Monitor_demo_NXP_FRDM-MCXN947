# Build And Flash

## Prerequisites
- Ubuntu bootstrap complete
- LinkServer installed and available on `PATH`

## Setup
```bash
./tools/setup_mcuxsdk_ws.sh
```

If local setup fails on optional middleware sync, use a known-good existing workspace:
```bash
WS_DIR=./mcuxsdk_ws
MCUX_EXAMPLES_DIR="$WS_DIR/mcuxsdk/examples" ./sdk_example/install_mcux_overlay.sh
```

## Build
```bash
./tools/build_frdmmcxn947.sh debug
```

Known-good build command used on 2026-02-15:
```bash
./tools/build_frdmmcxn947.sh debug
```

## Flash
```bash
./tools/flash_frdmmcxn947.sh
```

Known-good flash command used on 2026-02-15:
```bash
./tools/flash_frdmmcxn947.sh
```

## Failsafe Restore
Golden failsafe restore command:
```bash
LinkServer flash --probe '#1' --update-mode none MCXN947:FRDM-MCXN947 \
  load --addr 0x0 failsafe/edgeai_ev_charger_monitor_demo_cm33_core0_GOLDEN_20260214_203015.bin
```

## Debug UART
```bash
ls -l /dev/serial/by-id
screen /dev/ttyACM0 115200
```

Expected marker:
- `EV Charger Monitor demo baseline booted`
