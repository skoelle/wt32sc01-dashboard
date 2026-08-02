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
PIO="$VENV_DIR/bin/pio"
if [ "$#" -ge 1 ]; then
    PORT="$1"
    echo "Using forced upload port: $PORT"
    "$PIO" run --target upload --upload-port "$PORT"
else
    echo "Auto-detecting upload port..."
    "$PIO" run --target upload
fi
echo "Build + flash complete."
