# WtyczkaVST - Przewodnik Dewelopera

## Jak mądrze rozwijać wtyczkę?

Development wtyczek audio różni się od zwykłych aplikacji. Oto najlepsze praktyki:

1. **Używaj "Audio Plugin Host":** Zamiast ładować Logic Pro za każdym razem, skompiluj i używaj `JUCE AudioPluginHost`. Jest lekki i pozwala na szybkie testowanie zmian w kodzie.
2. **Format Standalone:** W fazie tworzenia UI, używaj formatu Standalone (aplikacja .app). Jest najszybszy w uruchamianiu.
3. **Wątek Audio:** Pamiętaj, że `processBlock` działa w czasie rzeczywistym. Nigdy nie blokuj tego wątku (żadnych operacji na plikach, printów do konsoli czy alokacji pamięci).

## Debugowanie bezpośrednio w Logic Pro

Jeśli musisz przetestować specyficzne zachowanie w Logic Pro, wykonaj poniższe kroki:

### 1. Przygotowanie Projektu
Upewnij się, że budujesz projekt w trybie **Debug**:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### 2. Podpięcie Debuggera (Xcode)
Jeśli używasz Xcode (wygenerowanego przez CMake):
1. Otwórz projekt w Xcode: `open WtyczkaVST.xcodeproj`.
2. Uruchom **Logic Pro** normalnie.
3. W Xcode wybierz z menu: **Debug > Attach to Process > Logic Pro**.
4. Ustaw breakpointy w swoim kodzie (np. w `processBlock` lub `paint`).
5. W Logic Pro załaduj swoją wtyczkę na ścieżkę audio. Xcode zatrzyma wykonywanie programu w momencie trafienia na breakpoint.

### 3. Debugowanie bez Xcode (Terminal/LLDB)
Jeśli wolisz terminal:
1. Uruchom Logic Pro.
2. W terminalu wpisz: `lldb -p $(pgrep -x "Logic Pro X")`.
3. Wpisz `continue` (lub `c`), aby wznowić działanie Logic Pro.

## Czyszczenie Cache'u Wtyczek (macOS)
Jeśli Logic Pro nie widzi zmian we wtyczce mimo jej przebudowania, wyczyść cache AU:
```bash
killall -9 AudioComponentRegistrar
rm ~/Library/Caches/com.apple.audiounits.cache
```
Po tym zabiegu Logic Pro ponownie przeskanuje wszystkie wtyczki przy starcie.
