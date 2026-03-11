#pragma once

#include "PluginProcessor.h"
#include <array>
#include <juce_audio_utils/juce_audio_utils.h>

class WtyczkaVSTAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    WtyczkaVSTAudioProcessorEditor (WtyczkaVSTAudioProcessor&);
    ~WtyczkaVSTAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    WtyczkaVSTAudioProcessor& audioProcessor;
    juce::Label midiDebugLabel;
    juce::MidiKeyboardState midiKeyboardState;
    juce::MidiKeyboardComponent midiKeyboardComponent;
    std::array<bool, 128> displayedActiveNotes {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WtyczkaVSTAudioProcessorEditor)
};
