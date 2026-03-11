#include "PluginProcessor.h"
#include "PluginEditor.h"

WtyczkaVSTAudioProcessorEditor::WtyczkaVSTAudioProcessorEditor (WtyczkaVSTAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      midiKeyboardComponent (midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible (midiDebugLabel);
    midiDebugLabel.setJustificationType (juce::Justification::centred);
    midiDebugLabel.setText ("Brak MIDI", juce::dontSendNotification);
    addAndMakeVisible (midiKeyboardComponent);
    midiKeyboardComponent.setAvailableRange (24, 108);
    midiKeyboardComponent.setScrollButtonsVisible (false);
    setSize (700, 600);
    startTimerHz (20);
}

WtyczkaVSTAudioProcessorEditor::~WtyczkaVSTAudioProcessorEditor()
{
}

void WtyczkaVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (24.0f);
    g.drawFittedText ("Wtyczka VST - Setup JUCE", 20, 30, getWidth() - 40, 40, juce::Justification::centred, 1);
}

void WtyczkaVSTAudioProcessorEditor::resized()
{
    constexpr int keyboardHeight = 140;
    constexpr int contentPadding = 20;

    midiDebugLabel.setBounds (contentPadding, 90, getWidth() - (contentPadding * 2), 24);
    midiKeyboardComponent.setBounds (contentPadding,
                                     getHeight() - keyboardHeight - contentPadding,
                                     getWidth() - (contentPadding * 2),
                                     keyboardHeight);
}

void WtyczkaVSTAudioProcessorEditor::timerCallback()
{
    midiDebugLabel.setText (audioProcessor.getLastMidiDebugMessage(), juce::dontSendNotification);

    for (int midiNoteNumber = 0; midiNoteNumber < 128; ++midiNoteNumber)
    {
        const bool isActive = audioProcessor.isMidiNoteActive (midiNoteNumber);
        if (displayedActiveNotes[static_cast<std::size_t> (midiNoteNumber)] == isActive)
            continue;

        displayedActiveNotes[static_cast<std::size_t> (midiNoteNumber)] = isActive;

        if (isActive)
            midiKeyboardState.noteOn (1, midiNoteNumber, 1.0f);
        else
            midiKeyboardState.noteOff (1, midiNoteNumber, 0.0f);
    }
}
