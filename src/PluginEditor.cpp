#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
int chooseWheelPixels (const juce::MouseWheelDetails& wheel, float& remainder)
{
    const auto primaryDelta = std::abs (wheel.deltaX) > std::abs (wheel.deltaY) ? wheel.deltaX : wheel.deltaY;
    if (primaryDelta == 0.0f)
        return 0;

    remainder += primaryDelta * 90.0f;
    const auto pixelDelta = static_cast<int> (std::trunc (remainder));
    remainder -= static_cast<float> (pixelDelta);
    return pixelDelta;
}
}

ScrollableMidiKeyboardComponent::ScrollableMidiKeyboardComponent (juce::MidiKeyboardState& state)
    : juce::MidiKeyboardComponent (state, juce::MidiKeyboardComponent::horizontalKeyboard)
{
}

void ScrollableMidiKeyboardComponent::setTargetViewport (juce::Viewport* viewport) noexcept
{
    targetViewport = viewport;
}

void ScrollableMidiKeyboardComponent::mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (targetViewport == nullptr)
    {
        juce::MidiKeyboardComponent::mouseWheelMove (event, wheel);
        return;
    }

    const auto pixelDelta = chooseWheelPixels (wheel, wheelPixelRemainder);
    if (pixelDelta == 0)
    {
        juce::MidiKeyboardComponent::mouseWheelMove (event, wheel);
        return;
    }

    targetViewport->setViewPosition (targetViewport->getViewPositionX() - pixelDelta, 0);
}

KeyboardPanViewport::KeyboardPanViewport()
{
    setScrollBarsShown (false, false, true, true);
    setMouseCursor (juce::MouseCursor::DraggingHandCursor);
}

void KeyboardPanViewport::mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    const auto pixelDelta = chooseWheelPixels (wheel, wheelPixelRemainder);
    if (pixelDelta == 0)
    {
        juce::Viewport::mouseWheelMove (event, wheel);
        return;
    }

    setViewPosition (getViewPositionX() - pixelDelta, 0);
}

KeyboardGestureOverlay::KeyboardGestureOverlay (juce::Viewport& targetViewport)
    : viewport (targetViewport)
{
    setMouseCursor (juce::MouseCursor::DraggingHandCursor);
}

void KeyboardGestureOverlay::mouseDown (const juce::MouseEvent& event)
{
    lastDragX = event.getPosition().x;
}

void KeyboardGestureOverlay::mouseDrag (const juce::MouseEvent& event)
{
    const auto deltaX = event.getPosition().x - lastDragX;
    lastDragX = event.getPosition().x;
    viewport.setViewPosition (viewport.getViewPositionX() - deltaX, 0);
}

void KeyboardGestureOverlay::mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    juce::ignoreUnused (event);
    const auto pixelDelta = chooseWheelPixels (wheel, wheelPixelRemainder);
    if (pixelDelta == 0)
        return;

    viewport.setViewPosition (viewport.getViewPositionX() - pixelDelta, 0);
}

FirstJucePluginAudioProcessorEditor::FirstJucePluginAudioProcessorEditor (FirstJucePluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      midiKeyboardComponent (midiKeyboardState),
      keyboardGestureOverlay (midiKeyboardViewport)
{
    addAndMakeVisible (midiDebugLabel);
    midiDebugLabel.setJustificationType (juce::Justification::centred);
    midiDebugLabel.setText ("Brak MIDI", juce::dontSendNotification);
    midiKeyboardComponent.setAvailableRange (24, 127);
    midiKeyboardComponent.setScrollButtonsVisible (false);
    midiKeyboardComponent.setKeyWidth (16.0f);

    midiKeyboardViewport.setViewedComponent (&midiKeyboardComponent, false);
    midiKeyboardComponent.setTargetViewport (&midiKeyboardViewport);
    addAndMakeVisible (midiKeyboardViewport);
    addAndMakeVisible (keyboardGestureOverlay);
    audioProcessor.setEditorKeyboardState (&midiKeyboardState);

    midiKeyboardViewport.setViewPosition (0, 0);
    setSize (700, 600);
    startTimerHz (20);
}

FirstJucePluginAudioProcessorEditor::~FirstJucePluginAudioProcessorEditor()
{
    audioProcessor.setEditorKeyboardState (nullptr);
}

void FirstJucePluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (24.0f);
    g.drawFittedText ("FirstJucePlugin - Setup JUCE", 20, 30, getWidth() - 40, 40, juce::Justification::centred, 1);
}

void FirstJucePluginAudioProcessorEditor::resized()
{
    constexpr int keyboardHeight = 140;
    constexpr int keyboardDragHandleHeight = 18;
    constexpr int contentPadding = 20;

    midiDebugLabel.setBounds (contentPadding, 90, getWidth() - (contentPadding * 2), 24);

    const int keyboardAreaY = getHeight() - keyboardHeight - contentPadding;
    const int keyboardAreaWidth = getWidth() - (contentPadding * 2);
    const int keyboardViewportHeight = keyboardHeight - keyboardDragHandleHeight;

    keyboardGestureOverlay.setBounds (contentPadding,
                                      keyboardAreaY,
                                      keyboardAreaWidth,
                                      keyboardDragHandleHeight);

    midiKeyboardViewport.setBounds (contentPadding,
                                    keyboardAreaY + keyboardDragHandleHeight,
                                    keyboardAreaWidth,
                                    keyboardViewportHeight);

    const auto keyboardWidth = juce::jmax (midiKeyboardViewport.getWidth(),
                                           static_cast<int> (std::round (midiKeyboardComponent.getTotalKeyboardWidth())));
    midiKeyboardComponent.setBounds (0, 0, keyboardWidth, keyboardViewportHeight);
    keyboardGestureOverlay.toFront (false);
}

void FirstJucePluginAudioProcessorEditor::timerCallback()
{
    midiDebugLabel.setText (audioProcessor.getLastMidiDebugMessage(), juce::dontSendNotification);
}
