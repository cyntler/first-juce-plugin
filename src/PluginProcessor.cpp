#include "PluginProcessor.h"
#include "PluginEditor.h"

FirstJucePluginAudioProcessor::FirstJucePluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
{
    // Dodajemy głosy do syntezatora
    for (int i = 0; i < numVoices; ++i)
        synth.addVoice (new SineWaveVoice());

    // Dodajemy definicję dźwięku
    synth.addSound (new SineWaveSound());
}

FirstJucePluginAudioProcessor::~FirstJucePluginAudioProcessor()
{
}

const juce::String FirstJucePluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool FirstJucePluginAudioProcessor::acceptsMidi() const { return true; }
bool FirstJucePluginAudioProcessor::producesMidi() const { return false; }
bool FirstJucePluginAudioProcessor::isMidiEffect() const { return false; }
double FirstJucePluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int FirstJucePluginAudioProcessor::getNumPrograms() { return 1; }
int FirstJucePluginAudioProcessor::getCurrentProgram() { return 0; }
void FirstJucePluginAudioProcessor::setCurrentProgram (int index) {}
const juce::String FirstJucePluginAudioProcessor::getProgramName (int index) { return {}; }
void FirstJucePluginAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void FirstJucePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Ustawiamy częstotliwość próbkowania dla syntezatora
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void FirstJucePluginAudioProcessor::releaseResources() {}

bool FirstJucePluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void FirstJucePluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    // To jest synth bez wejscia audio, wiec kazdy blok musi zaczynac od czystego bufora.
    buffer.clear();

    juce::MidiBuffer filteredMidi;

    for (const auto metadata : midiMessages)
    {
        const auto& message = metadata.getMessage();
        // Nie podtrzymujemy nut sustain pedalem podczas podstawowych testow.
        if (message.isController() && message.getControllerNumber() == 64)
            continue;

        filteredMidi.addEvent (message, metadata.samplePosition);
    }

    if (auto* keyboardState = editorKeyboardState.load())
        keyboardState->processNextMidiBuffer (filteredMidi, 0, buffer.getNumSamples(), true);

    for (const auto metadata : filteredMidi)
    {
        const auto& message = metadata.getMessage();
        updateMidiDebugMessage (message);

        if (message.isNoteOn())
            setMidiNoteActive (message.getNoteNumber(), true);
        else if (message.isNoteOff())
            setMidiNoteActive (message.getNoteNumber(), false);
        else if (message.isAllNotesOff() || message.isAllSoundOff())
            clearAllMidiNotes();
    }

    // Renderujemy dźwięk z syntezatora na podstawie komunikatów MIDI
    synth.renderNextBlock (buffer, filteredMidi, 0, buffer.getNumSamples());
}

bool FirstJucePluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* FirstJucePluginAudioProcessor::createEditor() { return new FirstJucePluginAudioProcessorEditor (*this); }

void FirstJucePluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void FirstJucePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::String FirstJucePluginAudioProcessor::getLastMidiDebugMessage() const
{
    const auto status = lastMidiStatusByte.load();
    const auto data1 = lastMidiData1.load();
    const auto data2 = lastMidiData2.load();
    const auto channel = lastMidiChannel.load();

    if (status == 0)
        return "Brak MIDI";

    return "Status 0x" + juce::String::toHexString (status).paddedLeft ('0', 2).toUpperCase()
        + " Ch " + juce::String (channel)
        + " D1 " + juce::String (data1)
        + " D2 " + juce::String (data2);
}

bool FirstJucePluginAudioProcessor::isMidiNoteActive (int midiNoteNumber) const noexcept
{
    if (midiNoteNumber < 0 || midiNoteNumber > 127)
        return false;

    const std::uint64_t noteMask = std::uint64_t { 1 } << (midiNoteNumber % 64);

    if (midiNoteNumber < 64)
        return (activeMidiNotesLow.load() & noteMask) != 0;

    return (activeMidiNotesHigh.load() & noteMask) != 0;
}

void FirstJucePluginAudioProcessor::setEditorKeyboardState (juce::MidiKeyboardState* keyboardState) noexcept
{
    editorKeyboardState.store (keyboardState);
}

void FirstJucePluginAudioProcessor::updateMidiDebugMessage (const juce::MidiMessage& message)
{
    const auto* raw = message.getRawData();
    const auto size = message.getRawDataSize();

    if (size > 0)
        lastMidiStatusByte.store (raw[0]);
    if (size > 1)
        lastMidiData1.store (raw[1]);
    if (size > 2)
        lastMidiData2.store (raw[2]);

    lastMidiChannel.store (message.getChannel());
}

void FirstJucePluginAudioProcessor::setMidiNoteActive (int midiNoteNumber, bool isActive) noexcept
{
    if (midiNoteNumber < 0 || midiNoteNumber > 127)
        return;

    const std::uint64_t noteMask = std::uint64_t { 1 } << (midiNoteNumber % 64);

    if (midiNoteNumber < 64)
    {
        auto current = activeMidiNotesLow.load();
        activeMidiNotesLow.store (isActive ? (current | noteMask) : (current & ~noteMask));
        return;
    }

    auto current = activeMidiNotesHigh.load();
    activeMidiNotesHigh.store (isActive ? (current | noteMask) : (current & ~noteMask));
}

void FirstJucePluginAudioProcessor::clearAllMidiNotes() noexcept
{
    activeMidiNotesLow.store (0);
    activeMidiNotesHigh.store (0);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new FirstJucePluginAudioProcessor(); }
