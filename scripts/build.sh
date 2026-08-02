#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
VENV_DIR=".venv-platformio"
if [ ! -d "$VENV_DIR" ]; then
    echo "Creating local virtualenv in $VENV_DIR ..."
    python3 -m venv "$VENV_DIR"
    "$VENV_DIR/bin/pip" install --upgrade pip
    "$VENV_DIR/bin/pip" install platformio
fi
echo "Running build (no upload) ..."
"$VENV_DIR/bin/pio" run
echo "Build finished successfully."
