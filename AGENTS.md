# AGENTS.md

Ten plik jest jedynym źródłem instrukcji dla agentów/modeli pracujących w tym repozytorium.

## Single Source of Truth
- Wszystkie modele (w tym Claude i Gemini) mają czytać zasady i kontekst projektu z tego pliku.
- Pliki `CLAUDE.md`, `GEMINI.md` i `PROJECT_CONTEXT.md` są wygaszone i zawierają wyłącznie odesłanie do `AGENTS.md`.
- W razie rozbieżności obowiązuje `AGENTS.md`.

## Project Context
- Repozytorium: `wtyczka-vst`
- Typ projektu: syntezator MIDI oparty o JUCE i CMake na macOS
- Aktualny status:
  - 8-głosowy syntezator sinus z obwiednią ADSR
  - formaty: `VST3`, `AU`, `Standalone`
  - brak parametrów automatyzowalnych, presetów i trwałego stanu
- Kluczowe technologie:
  - C++17
  - CMake `3.22+`
  - JUCE `8.0.12` (FetchContent w `CMakeLists.txt`)

## Build and Run
- Szybki build Standalone: `./scripts/dev-build.sh`
- Watch/rebuild: `./scripts/dev-watch.sh`
- Build + debug: `./scripts/dev-debug.sh`
- Manualnie:
  - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
  - `cmake --build build --config Debug`
- Artefakty: `build/WtyczkaVST_artefacts/`

## Code Map
- `src/PluginProcessor.h` i `src/PluginProcessor.cpp`:
  - `SineWaveSound`
  - `SineWaveVoice`
  - `WtyczkaVSTAudioProcessor`
- `src/PluginEditor.h` i `src/PluginEditor.cpp`:
  - `WtyczkaVSTAudioProcessorEditor` (UI min. 700x600, timer do odczytu informacji MIDI, wizualizacja klawiatury)

## Audio Thread Safety
- `processBlock()` działa na wątku realtime audio.
- Zakazane w `processBlock()`:
  - alokacje pamięci
  - I/O plikowe
  - logowanie do konsoli
  - blokady (`mutex`, locki)
- Do komunikacji audio/UI używaj mechanizmów bezpiecznych dla wątków (np. `std::atomic`).

## Development Rules
- Wprowadzaj małe, precyzyjne zmiany.
- Zachowuj istniejący styl kodu i strukturę projektu.
- Nie modyfikuj niepowiązanych plików.
- Używaj opisowych nazw i unikaj zbędnych zależności.
- Komentarze dodawaj tylko tam, gdzie logika nie jest oczywista.

## Git Safety
- Nie uruchamiaj destrukcyjnych komend git bez wyraźnej prośby użytkownika.
- Nie twórz commitów ani branchy bez wyraźnej prośby użytkownika.
- Szanuj lokalne, niescommitowane zmiany użytkownika.

## Testing and Validation
- Najpierw uruchamiaj najmniejszy test/check związany ze zmianą.
- Potem ewentualnie szerszą walidację.
- Jeśli nie da się uruchomić testów, jasno opisz czego nie zweryfikowano.

## macOS Plugin Notes
- Standardowe ścieżki instalacji:
  - AU: `~/Library/Audio/Plug-Ins/Components/`
  - VST3: `~/Library/Audio/Plug-Ins/VST3/`
- Gdy Logic Pro nie widzi zmian AU:
  - `killall -9 AudioComponentRegistrar`
  - `rm ~/Library/Caches/com.apple.audiounits.cache`

## Handoff
- Na koniec pracy podaj:
  - co zostało zmienione
  - dlaczego
  - co warto zrobić dalej
