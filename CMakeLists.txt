cmake_minimum_required(VERSION 3.22)

project(FirstJucePlugin VERSION 0.1.0 LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pobieranie JUCE
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.12 # Nowsza stabilna wersja potrzebna dla macOS 15 SDK
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(FirstJucePlugin
    COMPANY_NAME "MojaFirma"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_STEP_LABEL "Copying FirstJucePlugin to Plug-Ins folder"
    PLUGIN_MANUFACTURER_CODE "Abcd" # 4 znaki, AU wymaga co najmniej jednej wielkiej litery
    PLUGIN_CODE "Efgh"             # 4 znaki
    FORMATS VST3 AU Standalone     # Standalone przyspiesza lokalne testy na macOS
    PRODUCT_NAME "FirstJucePlugin"
)

target_sources(FirstJucePlugin PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h
)

target_compile_definitions(FirstJucePlugin
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(FirstJucePlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_extra
)

juce_generate_juce_header(FirstJucePlugin)
