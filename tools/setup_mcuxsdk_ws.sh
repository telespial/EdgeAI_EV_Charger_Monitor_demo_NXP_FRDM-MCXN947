#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WS_DIR="${WS_DIR:-$ROOT_DIR/mcuxsdk_ws}"
MCUX_MANIFEST_URL="${MCUX_MANIFEST_URL:-https://github.com/nxp-mcuxpresso/mcuxsdk-manifests.git}"
MCUX_MANIFEST_REV="${MCUX_MANIFEST_REV:-a2a4de055a35e20ea7de66f3afe4303f6fdc0934}"

mkdir -p "$WS_DIR"

if [[ ! -d "$WS_DIR/.west" ]]; then
  (cd "$WS_DIR" && west init -m "$MCUX_MANIFEST_URL")
fi

(
  cd "$WS_DIR"
  if ! git -C manifests cat-file -e "$MCUX_MANIFEST_REV^{commit}" 2>/dev/null; then
    git -C manifests fetch -q origin
  fi
  git -C manifests checkout -q "$MCUX_MANIFEST_REV"
)

(
  cd "$WS_DIR"
  west update --fetch-opt=--depth=1 || west update
)

(
  cd "$ROOT_DIR"
  MCUX_EXAMPLES_DIR="$WS_DIR/mcuxsdk/examples" ./sdk_example/install_mcux_overlay.sh
)

"$ROOT_DIR/tools/patch_mcuxsdk.sh" "$WS_DIR"

echo "Workspace ready at: $WS_DIR"
