# FirstJucePlugin

Prosty syntezator MIDI oparty o JUCE i CMake (macOS).

## Co działa teraz

- 8-głosowy syntezator sinus z ADSR
- formaty: `VST3`, `AU`, `Standalone`
- wizualizacja klawiatury MIDI w GUI
- podświetlanie nut z wejścia MIDI
- granie nuty myszką (`LPM`) na klawiaturze GUI
- przewijanie klawiatury (drag + wheel)

## Wymagania

- macOS
- CMake `3.22+`
- kompilator z obsługą `C++17`

JUCE (`8.0.12`) pobiera się automatycznie przez `FetchContent` w `CMakeLists.txt`.

## Szybki start

### Build Standalone (Debug)

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

## Ręcznie przez CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

Artefakty znajdziesz w:

`build/FirstJucePlugin_artefacts/`

## Struktura kodu

- `src/PluginProcessor.h`, `src/PluginProcessor.cpp` – synteza i obsługa MIDI
- `src/PluginEditor.h`, `src/PluginEditor.cpp` – interfejs i klawiatura GUI
- `AGENTS.md` – zasady pracy w repozytorium

## Uwaga o wątku audio

W `processBlock()` unikaj:

- alokacji pamięci
- I/O plikowego
- logowania do konsoli
- blokad (`mutex`, locki)

## macOS: gdy host nie widzi zmian AU

```bash
killall -9 AudioComponentRegistrar
rm ~/Library/Caches/com.apple.audiounits.cache
```
