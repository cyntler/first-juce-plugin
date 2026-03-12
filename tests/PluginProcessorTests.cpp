#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "../src/PluginProcessor.h"

static std::unique_ptr<FirstJucePluginAudioProcessor> createTestProcessor()
{
    auto processor = std::make_unique<FirstJucePluginAudioProcessor>();
    processor->prepareToPlay (44100.0, 512);
    return processor;
}

static void processMidi (FirstJucePluginAudioProcessor& processor,
                         const juce::MidiBuffer& midi,
                         int numSamples = 512)
{
    juce::AudioBuffer<float> buffer (2, numSamples);
    buffer.clear();
    juce::MidiBuffer midiCopy (midi);
    processor.processBlock (buffer, midiCopy);
}

// =============================================================================
// MIDI note bitmask tracking
// =============================================================================

TEST_CASE ("MIDI note tracking via processBlock", "[midi][bitmask]")
{
    auto processor = createTestProcessor();

    SECTION ("No notes active initially")
    {
        for (int note = 0; note <= 127; ++note)
            REQUIRE (processor->isMidiNoteActive (note) == false);
    }

    SECTION ("Note on sets note active")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.8f), 0);
        processMidi (*processor, midi);

        REQUIRE (processor->isMidiNoteActive (60) == true);
        REQUIRE (processor->isMidiNoteActive (59) == false);
        REQUIRE (processor->isMidiNoteActive (61) == false);
    }

    SECTION ("Note off clears note active")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.8f), 0);
        processMidi (*processor, midi);
        REQUIRE (processor->isMidiNoteActive (60) == true);

        juce::MidiBuffer midiOff;
        midiOff.addEvent (juce::MidiMessage::noteOff (1, 60), 0);
        processMidi (*processor, midiOff);
        REQUIRE (processor->isMidiNoteActive (60) == false);
    }

    SECTION ("Multiple notes in same block")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 36, 0.5f), 0);
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.5f), 10);
        midi.addEvent (juce::MidiMessage::noteOn (1, 96, 0.5f), 20);
        processMidi (*processor, midi);

        REQUIRE (processor->isMidiNoteActive (36) == true);
        REQUIRE (processor->isMidiNoteActive (60) == true);
        REQUIRE (processor->isMidiNoteActive (96) == true);
    }

    SECTION ("Boundary notes: 0 and 127")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 0, 0.5f), 0);
        midi.addEvent (juce::MidiMessage::noteOn (1, 127, 0.5f), 10);
        processMidi (*processor, midi);

        REQUIRE (processor->isMidiNoteActive (0) == true);
        REQUIRE (processor->isMidiNoteActive (127) == true);
    }

    SECTION ("Low/high bitmask boundary: notes 63 and 64")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 63, 0.5f), 0);
        midi.addEvent (juce::MidiMessage::noteOn (1, 64, 0.5f), 10);
        processMidi (*processor, midi);

        REQUIRE (processor->isMidiNoteActive (63) == true);
        REQUIRE (processor->isMidiNoteActive (64) == true);
        REQUIRE (processor->isMidiNoteActive (62) == false);
        REQUIRE (processor->isMidiNoteActive (65) == false);
    }

    SECTION ("All notes off clears all notes")
    {
        juce::MidiBuffer midiOn;
        midiOn.addEvent (juce::MidiMessage::noteOn (1, 20, 0.5f), 0);
        midiOn.addEvent (juce::MidiMessage::noteOn (1, 60, 0.5f), 10);
        midiOn.addEvent (juce::MidiMessage::noteOn (1, 100, 0.5f), 20);
        processMidi (*processor, midiOn);

        juce::MidiBuffer midiAllOff;
        midiAllOff.addEvent (juce::MidiMessage::allNotesOff (1), 0);
        processMidi (*processor, midiAllOff);

        REQUIRE (processor->isMidiNoteActive (20) == false);
        REQUIRE (processor->isMidiNoteActive (60) == false);
        REQUIRE (processor->isMidiNoteActive (100) == false);
    }

    SECTION ("All sound off clears all notes")
    {
        juce::MidiBuffer midiOn;
        midiOn.addEvent (juce::MidiMessage::noteOn (1, 72, 0.5f), 0);
        processMidi (*processor, midiOn);
        REQUIRE (processor->isMidiNoteActive (72) == true);

        juce::MidiBuffer midiAllSoundOff;
        midiAllSoundOff.addEvent (juce::MidiMessage::allSoundOff (1), 0);
        processMidi (*processor, midiAllSoundOff);

        REQUIRE (processor->isMidiNoteActive (72) == false);
    }
}

TEST_CASE ("isMidiNoteActive rejects out-of-range notes", "[midi][bitmask]")
{
    auto processor = createTestProcessor();

    REQUIRE (processor->isMidiNoteActive (-1) == false);
    REQUIRE (processor->isMidiNoteActive (128) == false);
    REQUIRE (processor->isMidiNoteActive (-100) == false);
    REQUIRE (processor->isMidiNoteActive (1000) == false);
}

// =============================================================================
// MIDI debug message formatting
// =============================================================================

TEST_CASE ("MIDI debug message formatting", "[midi][debug]")
{
    auto processor = createTestProcessor();

    SECTION ("Default message before any MIDI")
    {
        REQUIRE (processor->getLastMidiDebugMessage() == "Brak MIDI");
    }

    SECTION ("Note on message formatting")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, (juce::uint8) 100), 0);
        processMidi (*processor, midi);

        auto msg = processor->getLastMidiDebugMessage();
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("Status 0x90"));
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("Ch 1"));
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("D1 60"));
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("D2 100"));
    }

    SECTION ("Note off message formatting")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOff (1, 60, (juce::uint8) 0), 0);
        processMidi (*processor, midi);

        auto msg = processor->getLastMidiDebugMessage();
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("Status 0x80"));
    }

    SECTION ("Different channel")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::noteOn (10, 36, 0.5f), 0);
        processMidi (*processor, midi);

        auto msg = processor->getLastMidiDebugMessage();
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("Ch 10"));
    }
}

// =============================================================================
// Sustain pedal CC64 filtering
// =============================================================================

TEST_CASE ("processBlock filters sustain pedal CC64", "[midi][filter]")
{
    auto processor = createTestProcessor();

    SECTION ("CC64 does not update MIDI debug message")
    {
        juce::MidiBuffer midiNote;
        midiNote.addEvent (juce::MidiMessage::noteOn (1, 60, 0.8f), 0);
        processMidi (*processor, midiNote);
        auto msgAfterNote = processor->getLastMidiDebugMessage();

        juce::MidiBuffer midiCC;
        midiCC.addEvent (juce::MidiMessage::controllerEvent (1, 64, 127), 0);
        processMidi (*processor, midiCC);

        REQUIRE (processor->getLastMidiDebugMessage() == msgAfterNote);
    }

    SECTION ("Other CCs are not filtered")
    {
        juce::MidiBuffer midi;
        midi.addEvent (juce::MidiMessage::controllerEvent (1, 1, 64), 0);
        processMidi (*processor, midi);

        auto msg = processor->getLastMidiDebugMessage();
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("Status 0xB0"));
        CHECK_THAT (msg.toStdString(), Catch::Matchers::ContainsSubstring ("D1 1"));
    }
}
