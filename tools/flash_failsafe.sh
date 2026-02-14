#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <failsafe-elf-or-bin>" >&2
  exit 1
fi

ARTIFACT="$1"
if [[ ! -f "$ARTIFACT" ]]; then
  echo "Artifact not found: $ARTIFACT" >&2
  exit 1
fi

if ! command -v LinkServer >/dev/null 2>&1; then
  echo "LinkServer not found on PATH." >&2
  exit 1
fi

LinkServer flash --probe '#1' MCXN947:FRDM-MCXN947 load --addr 0x0 "$ARTIFACT"
