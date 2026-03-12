#include <catch2/catch_test_macros.hpp>
#include "../src/PluginProcessor.h"

TEST_CASE ("SineWaveVoice canPlaySound", "[voice]")
{
    SineWaveVoice voice;
    SineWaveSound sineSound;

    SECTION ("Can play SineWaveSound")
    {
        REQUIRE (voice.canPlaySound (&sineSound) == true);
    }

    SECTION ("Cannot play nullptr")
    {
        REQUIRE (voice.canPlaySound (nullptr) == false);
    }
}

TEST_CASE ("SineWaveVoice renders silence when not started", "[voice][render]")
{
    juce::Synthesiser synth;
    synth.addVoice (new SineWaveVoice());
    synth.addSound (new SineWaveSound());
    synth.setCurrentPlaybackSampleRate (44100.0);

    juce::AudioBuffer<float> buffer (2, 512);
    buffer.clear();
    juce::MidiBuffer emptyMidi;

    synth.renderNextBlock (buffer, emptyMidi, 0, 512);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        for (int s = 0; s < buffer.getNumSamples(); ++s)
            REQUIRE (buffer.getSample (ch, s) == 0.0f);
}

TEST_CASE ("SineWaveVoice produces non-zero output for note on", "[voice][render]")
{
    juce::Synthesiser synth;
    synth.addVoice (new SineWaveVoice());
    synth.addSound (new SineWaveSound());
    synth.setCurrentPlaybackSampleRate (44100.0);

    juce::AudioBuffer<float> buffer (2, 512);
    buffer.clear();
    juce::MidiBuffer midi;
    midi.addEvent (juce::MidiMessage::noteOn (1, 69, 0.8f), 0);

    synth.renderNextBlock (buffer, midi, 0, 512);

    bool hasNonZero = false;
    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        if (buffer.getSample (0, s) != 0.0f)
        {
            hasNonZero = true;
            break;
        }
    }
    REQUIRE (hasNonZero);
}
