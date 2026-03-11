#!/bin/zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
APP_NAME="WtyczkaVST"
STAMP_FILE="$BUILD_DIR/.dev-watch.stamp"

find_app() {
  find "$BUILD_DIR" -path "*Standalone/*.app" -name "${APP_NAME}.app" | head -n 1
}

kill_running_app() {
  osascript -e "tell application \"$APP_NAME\" to quit" >/dev/null 2>&1 || true

  for _ in {1..20}; do
    if ! osascript -e "tell application \"System Events\" to exists process \"$APP_NAME\"" 2>/dev/null | grep -q "true"; then
      return 0
    fi

    sleep 0.25
  done

  pkill -x "$APP_NAME" >/dev/null 2>&1 || true
}

launch_app() {
  local app_path
  app_path="$(find_app)"
  if [[ -z "${app_path}" ]]; then
    echo "Nie znaleziono ${APP_NAME}.app po buildzie." >&2
    return 1
  fi

  open -n "$app_path"
}

needs_rebuild() {
  if [[ ! -f "$STAMP_FILE" ]]; then
    return 0
  fi

  [[ -n "$(find "$ROOT_DIR/src" "$ROOT_DIR/CMakeLists.txt" -newer "$STAMP_FILE" -print -quit 2>/dev/null)" ]]
}

mkdir -p "$BUILD_DIR"

echo "Start watch loop dla ${APP_NAME}. Zmien plik w src/ albo CMakeLists.txt, aby przebudowac i zrestartowac aplikacje."

while true; do
  if needs_rebuild; then
    echo
    echo "==> Build i restart: $(date '+%Y-%m-%d %H:%M:%S')"
    "$ROOT_DIR/scripts/dev-build.sh"
    touch "$STAMP_FILE"
    kill_running_app
    launch_app
  fi

  sleep 1
done
