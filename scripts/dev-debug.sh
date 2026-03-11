#!/bin/zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

find_app() {
  find "$BUILD_DIR" -path "*Standalone/*.app" -name "FirstJucePlugin.app" | head -n 1
}

"$ROOT_DIR/scripts/dev-build.sh"

APP_PATH="$(find_app)"
if [[ -z "${APP_PATH}" ]]; then
  echo "Nie znaleziono aplikacji Standalone po buildzie." >&2
  exit 1
fi

exec lldb "$APP_PATH/Contents/MacOS/FirstJucePlugin"
