#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WS_DIR="${1:-${WS_DIR:-$ROOT_DIR/mcuxsdk_ws}}"
KISS_FASTFIR_C="$WS_DIR/mcuxsdk/middleware/eiq/tensorflow-lite/third_party/kissfft/tools/kiss_fastfir.c"
EIQ_CMAKE="$WS_DIR/mcuxsdk/middleware/eiq/CMakeLists.txt"
MPP_DIR="$WS_DIR/mcuxsdk/middleware/eiq/mpp"

echo "[patch] ws: $WS_DIR"

if [[ -f "$KISS_FASTFIR_C" ]]; then
  if grep -q "__attribute__\\s*((" "$KISS_FASTFIR_C"; then
    echo "[patch] ok: kiss_fastfir.c already patched"
  elif grep -qE '^static int verbose=0;[[:space:]]*$' "$KISS_FASTFIR_C"; then
    perl -0777 -pi -e 's|^static int verbose=0;\\s*$|/* Patched for -Werror unused variable in upstream tools file. */\\nstatic int verbose __attribute__((unused)) = 0;|m' "$KISS_FASTFIR_C"
    echo "[patch] fixed kiss_fastfir.c"
  fi
fi

# Some SDK snapshots pin an MPP commit that is no longer available upstream.
# Keep the project buildable by skipping mpp integration when the module is missing.
if [[ -f "$EIQ_CMAKE" ]]; then
  if grep -q "edgeai_skip_missing_mpp_guard" "$EIQ_CMAKE"; then
    echo "[patch] ok: eiq CMakeLists already guarded for missing mpp"
  elif grep -q "mcux_add_cmakelists(\\\${SdkRootDirPath}/middleware/eiq/mpp)" "$EIQ_CMAKE"; then
    perl -0777 -pi -e 's|mcux_add_cmakelists\(\$\{SdkRootDirPath\}/middleware/eiq/mpp\)|# edgeai_skip_missing_mpp_guard\nif(EXISTS ${SdkRootDirPath}/middleware/eiq/mpp/CMakeLists.txt)\nmcux_add_cmakelists(${SdkRootDirPath}/middleware/eiq/mpp)\nelse()\n  message(STATUS "Skipping middleware/eiq/mpp (missing CMakeLists.txt)")\nendif()|g' "$EIQ_CMAKE"
    echo "[patch] guarded missing mpp in eiq/CMakeLists.txt"
  fi
fi

if [[ -d "$MPP_DIR/.git" ]] && ! git -C "$MPP_DIR" rev-parse --verify HEAD >/dev/null 2>&1; then
  echo "[patch] warning: mpp repo has no commits; attempting external_main checkout"
  git -C "$MPP_DIR" fetch -q nxp-mcuxpresso external_main || true
  git -C "$MPP_DIR" checkout -q FETCH_HEAD || true
fi
