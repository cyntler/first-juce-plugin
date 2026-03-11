#pragma once

#include "PluginProcessor.h"
#include <juce_audio_utils/juce_audio_utils.h>

class KeyboardPanViewport : public juce::Viewport
{
public:
    KeyboardPanViewport();
    void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

private:
    float wheelPixelRemainder = 0.0f;
};

class ScrollableMidiKeyboardComponent : public juce::MidiKeyboardComponent
{
public:
    explicit ScrollableMidiKeyboardComponent (juce::MidiKeyboardState& state);
    void setTargetViewport (juce::Viewport* viewport) noexcept;
    void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

private:
    juce::Viewport* targetViewport = nullptr;
    float wheelPixelRemainder = 0.0f;
};

class KeyboardGestureOverlay : public juce::Component
{
public:
    explicit KeyboardGestureOverlay (juce::Viewport& targetViewport);

    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

private:
    juce::Viewport& viewport;
    int lastDragX = 0;
    float wheelPixelRemainder = 0.0f;
};

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
    ScrollableMidiKeyboardComponent midiKeyboardComponent;
    KeyboardPanViewport midiKeyboardViewport;
    KeyboardGestureOverlay keyboardGestureOverlay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WtyczkaVSTAudioProcessorEditor)
};
