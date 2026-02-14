#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WS_DIR="${1:-${WS_DIR:-$ROOT_DIR/mcuxsdk_ws}}"
KISS_FASTFIR_C="$WS_DIR/mcuxsdk/middleware/eiq/tensorflow-lite/third_party/kissfft/tools/kiss_fastfir.c"

echo "[patch] ws: $WS_DIR"

if [[ -f "$KISS_FASTFIR_C" ]]; then
  if grep -q "__attribute__\\s*((" "$KISS_FASTFIR_C"; then
    echo "[patch] ok: kiss_fastfir.c already patched"
  elif grep -qE '^static int verbose=0;[[:space:]]*$' "$KISS_FASTFIR_C"; then
    perl -0777 -pi -e 's|^static int verbose=0;\\s*$|/* Patched for -Werror unused variable in upstream tools file. */\\nstatic int verbose __attribute__((unused)) = 0;|m' "$KISS_FASTFIR_C"
    echo "[patch] fixed kiss_fastfir.c"
  fi
fi
