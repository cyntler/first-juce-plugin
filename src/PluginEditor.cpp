#include "PluginProcessor.h"
#include "PluginEditor.h"

WtyczkaVSTAudioProcessorEditor::WtyczkaVSTAudioProcessorEditor (WtyczkaVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible (midiDebugLabel);
    midiDebugLabel.setJustificationType (juce::Justification::centred);
    midiDebugLabel.setText ("Brak MIDI", juce::dontSendNotification);
    setSize (400, 300);
    startTimerHz (20);
}

WtyczkaVSTAudioProcessorEditor::~WtyczkaVSTAudioProcessorEditor()
{
}

void WtyczkaVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Wtyczka VST - Setup JUCE", 20, 70, getWidth() - 40, 30, juce::Justification::centred, 1);
}

void WtyczkaVSTAudioProcessorEditor::resized()
{
    midiDebugLabel.setBounds (20, 150, getWidth() - 40, 24);
}

void WtyczkaVSTAudioProcessorEditor::timerCallback()
{
    midiDebugLabel.setText (audioProcessor.getLastMidiDebugMessage(), juce::dontSendNotification);
}
