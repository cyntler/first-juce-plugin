#!/bin/zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build "$BUILD_DIR" --config Debug --target FirstJucePluginTests

cd "$BUILD_DIR"
ctest --output-on-failure -C Debug
