# FirstJucePlugin

A simple MIDI synthesiser built with JUCE and CMake (macOS).

## What works now

- 8-voice sine synthesiser with ADSR
- formats: `VST3`, `AU`, `Standalone`
- MIDI keyboard visualisation in the GUI
- note highlighting from MIDI input
- playing notes with the mouse (left click) on the GUI keyboard
- keyboard scrolling (drag + wheel)

## Requirements

- macOS
- CMake `3.22+`
- a compiler with `C++17` support

JUCE (`8.0.12`) is fetched automatically via `FetchContent` in `CMakeLists.txt`.

## Quick start

### Standalone build (Debug)

```bash
./scripts/dev-build.sh
```

### Watch / auto-rebuild

```bash
./scripts/dev-watch.sh
```

### Build + debug (LLDB)

```bash
./scripts/dev-debug.sh
```

## Manual CMake build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

Build artefacts can be found in:

`build/FirstJucePlugin_artefacts/`

## Code structure

- `src/PluginProcessor.h`, `src/PluginProcessor.cpp` – synthesis and MIDI handling
- `src/PluginEditor.h`, `src/PluginEditor.cpp` – interface and GUI keyboard
- `AGENTS.md` – repository working guidelines

## Audio thread note

Avoid the following in `processBlock()`:

- memory allocations
- file I/O
- console logging
- locks (`mutex`, etc.)

## macOS: when the host does not detect AU changes

```bash
killall -9 AudioComponentRegistrar
rm ~/Library/Caches/com.apple.audiounits.cache
```
