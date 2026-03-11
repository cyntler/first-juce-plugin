#pragma once

#include "PluginProcessor.h"

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WtyczkaVSTAudioProcessorEditor)
};
