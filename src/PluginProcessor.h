#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>

// --- Prosty dźwięk (Sound) dla syntezatora ---
class SineWaveSound : public juce::SynthesiserSound
{
public:
    SineWaveSound() {}
    bool appliesToNote (int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel (int /*midiChannel*/) override { return true; }
};

// --- Prosty głos (Voice) dla syntezatora ---
class SineWaveVoice : public juce::SynthesiserVoice
{
public:
    SineWaveVoice()
    {
        adsrParams.attack = 0.01f;
        adsrParams.decay = 0.05f;
        adsrParams.sustain = 0.8f;
        adsrParams.release = 0.15f;
        adsr.setParameters (adsrParams);
    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        level = velocity * 0.15f; // Głośność
        phase = 0.0;
        tailOff = 0.0;
        adsr.setSampleRate (getSampleRate());
        adsr.noteOn();
        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            adsr.reset();
            clearCurrentNote();
            angleDelta = 0.0;
            tailOff = 0.0;
        }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            while (--numSamples >= 0)
            {
                const auto env = adsr.getNextSample();
                auto currentSample = (float) (std::sin (phase) * level * env);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);

                phase += angleDelta;
                startSample++;

                if (! adsr.isActive())
                {
                    adsr.reset();
                    clearCurrentNote();
                    angleDelta = 0.0;
                    tailOff = 0.0;
                    break;
                }
            }
        }
    }

private:
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    double phase = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};

// --- Główny Audio Processor ---
class WtyczkaVSTAudioProcessor  : public juce::AudioProcessor
{
public:
    WtyczkaVSTAudioProcessor();
    ~WtyczkaVSTAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::String getLastMidiDebugMessage() const;

private:
    void updateMidiDebugMessage (const juce::MidiMessage& message);

    juce::Synthesiser synth;
    const int numVoices = 8;
    std::atomic<int> lastMidiStatusByte { 0 };
    std::atomic<int> lastMidiData1 { 0 };
    std::atomic<int> lastMidiData2 { 0 };
    std::atomic<int> lastMidiChannel { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WtyczkaVSTAudioProcessor)
};
