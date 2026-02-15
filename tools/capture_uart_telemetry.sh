#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="$ROOT_DIR/captures"
UART_DEV="${UART_DEV:-/dev/ttyACM0}"
DURATION="${DURATION:-60}"
BAUD="${BAUD:-115200}"
OUT_FILE=""

usage() {
  cat <<EOF
Usage: $0 [--uart /dev/ttyACM0] [--duration 60] [--baud 115200] [--out captures/file.csv]
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --uart)
      UART_DEV="$2"
      shift 2
      ;;
    --duration)
      DURATION="$2"
      shift 2
      ;;
    --baud)
      BAUD="$2"
      shift 2
      ;;
    --out)
      OUT_FILE="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $1" >&2
      usage
      exit 2
      ;;
  esac
done

mkdir -p "$OUT_DIR"
if [[ -z "$OUT_FILE" ]]; then
  OUT_FILE="$OUT_DIR/uart_telemetry_$(date +%Y%m%d_%H%M%S).csv"
fi

if [[ ! -e "$UART_DEV" ]]; then
  echo "UART device does not exist: $UART_DEV" >&2
  exit 1
fi

stty -F "$UART_DEV" "$BAUD" cs8 -cstopb -parenb -ixon -ixoff -echo raw

echo "t_ms,current_mA,power_mW,voltage_mV,temp_c,soc_pct,mode" > "$OUT_FILE"
echo "[capture] uart=$UART_DEV baud=$BAUD duration=${DURATION}s out=$OUT_FILE"

timeout "${DURATION}s" stdbuf -oL cat "$UART_DEV" | \
  awk -F',' '
    /^TELEM_CSV,/ && NF >= 8 {
      print $2 "," $3 "," $4 "," $5 "," $6 "," $7 "," $8
      fflush(stdout)
    }' >> "$OUT_FILE" || true

echo "[capture] done: $OUT_FILE"
