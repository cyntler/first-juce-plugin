#include "PluginProcessor.h"
#include "PluginEditor.h"

WtyczkaVSTAudioProcessor::WtyczkaVSTAudioProcessor()
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

WtyczkaVSTAudioProcessor::~WtyczkaVSTAudioProcessor()
{
}

const juce::String WtyczkaVSTAudioProcessor::getName() const { return JucePlugin_Name; }
bool WtyczkaVSTAudioProcessor::acceptsMidi() const { return true; }
bool WtyczkaVSTAudioProcessor::producesMidi() const { return false; }
bool WtyczkaVSTAudioProcessor::isMidiEffect() const { return false; }
double WtyczkaVSTAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int WtyczkaVSTAudioProcessor::getNumPrograms() { return 1; }
int WtyczkaVSTAudioProcessor::getCurrentProgram() { return 0; }
void WtyczkaVSTAudioProcessor::setCurrentProgram (int index) {}
const juce::String WtyczkaVSTAudioProcessor::getProgramName (int index) { return {}; }
void WtyczkaVSTAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void WtyczkaVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Ustawiamy częstotliwość próbkowania dla syntezatora
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void WtyczkaVSTAudioProcessor::releaseResources() {}

bool WtyczkaVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void WtyczkaVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    // To jest synth bez wejscia audio, wiec kazdy blok musi zaczynac od czystego bufora.
    buffer.clear();

    juce::MidiBuffer filteredMidi;

    for (const auto metadata : midiMessages)
    {
        const auto& message = metadata.getMessage();
        updateMidiDebugMessage (message);

        // Nie podtrzymujemy nut sustain pedalem podczas podstawowych testow.
        if (message.isController() && message.getControllerNumber() == 64)
            continue;

        if (message.isAllNotesOff() || message.isAllSoundOff())
            synth.allNotesOff (message.getChannel(), true);

        filteredMidi.addEvent (message, metadata.samplePosition);
    }

    // Renderujemy dźwięk z syntezatora na podstawie komunikatów MIDI
    synth.renderNextBlock (buffer, filteredMidi, 0, buffer.getNumSamples());
}

bool WtyczkaVSTAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* WtyczkaVSTAudioProcessor::createEditor() { return new WtyczkaVSTAudioProcessorEditor (*this); }

void WtyczkaVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void WtyczkaVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::String WtyczkaVSTAudioProcessor::getLastMidiDebugMessage() const
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

void WtyczkaVSTAudioProcessor::updateMidiDebugMessage (const juce::MidiMessage& message)
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

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new WtyczkaVSTAudioProcessor(); }
