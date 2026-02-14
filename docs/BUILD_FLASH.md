# Build And Flash

## Prerequisites
- Ubuntu bootstrap complete
- LinkServer installed and available on `PATH`

## Setup
```bash
./tools/setup_mcuxsdk_ws.sh
```

## Build
```bash
./tools/build_frdmmcxn947.sh debug
```

## Flash
```bash
./tools/flash_frdmmcxn947.sh
```

## Debug UART
```bash
ls -l /dev/serial/by-id
screen /dev/ttyACM0 115200
```

Expected marker:
- `EV Charger Monitor demo baseline booted`
