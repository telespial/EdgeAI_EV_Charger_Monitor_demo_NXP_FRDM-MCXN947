#!/usr/bin/env bash
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OVERLAY_DIR="$HERE/mcuxsdk_examples_overlay"
MCUX_EXAMPLES_DIR="${MCUX_EXAMPLES_DIR:-}"

if [[ ! -d "$OVERLAY_DIR/demo_apps/edgeai_ev_charger_monitor_demo" ]]; then
  echo "Overlay not found at: $OVERLAY_DIR" >&2
  exit 1
fi

if [[ ! -d "$MCUX_EXAMPLES_DIR" ]]; then
  echo "MCUX examples dir not found at: $MCUX_EXAMPLES_DIR" >&2
  echo "Set MCUX_EXAMPLES_DIR to <west-workspace>/mcuxsdk/examples." >&2
  exit 1
fi

mkdir -p "$MCUX_EXAMPLES_DIR/demo_apps" "$MCUX_EXAMPLES_DIR/_boards/frdmmcxn947/demo_apps"
cp -a "$OVERLAY_DIR/demo_apps/edgeai_ev_charger_monitor_demo" "$MCUX_EXAMPLES_DIR/demo_apps/"
cp -a "$OVERLAY_DIR/_boards/frdmmcxn947/demo_apps/edgeai_ev_charger_monitor_demo" "$MCUX_EXAMPLES_DIR/_boards/frdmmcxn947/demo_apps/"

echo "Installed edgeai_ev_charger_monitor_demo wrapper into: $MCUX_EXAMPLES_DIR"
