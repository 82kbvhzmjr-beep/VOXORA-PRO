#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Dsp.h"

class VoxoraProcessor : public juce::AudioProcessor
{
public:
    VoxoraProcessor();
    ~VoxoraProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vocodex Pro"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }

    int getNumPrograms() override;
    int getNumCategories() const noexcept;
    juce::String getCategoryName (int category) const;
    juce::Array<int> getPresetIndicesForCategory (int category) const;
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioProcessorValueTreeState apvts;

    // --- Datos para la interfaz (se escriben en el hilo de audio, se leen en el de UI) ---
    std::atomic<float> inPeak { 0.0f }, outPeak { 0.0f }, grMeter { 0.0f }, detectedMidi { -1.0f };

    // --- Asistente vocal: escucha ~6 s de voz y ajusta la cadena ---
    enum AssistState { assistIdle = 0, assistListening = 1, assistComputed = 2, assistApplied = 3, assistRestored = 4 };
    void startAssist();
    void commitAssist();                 // llamar desde el hilo de UI cuando el estado es assistComputed
    void setAssistApplied (bool applied);
    int getAssistState() const noexcept   { return assistState.load(); }
    float getAssistProgress() const noexcept { return assistProgress.load(); }

private:
    void analyseForAssist (const juce::AudioBuffer<float>& buf);
    void applyPreset (int index);

    // Cadena
    vx::Eq eq;
    vx::Compressor comp;
    vx::DeEsser deEss;
    vx::Saturator sat;
    vx::AirExciter air;
    vx::ReverbSend reverb;
    vx::DelaySend delay;
    vx::PitchDetector detector;
    vx::PitchShifter shifter;
    vx::MicEmulator micEmulator;
    juce::AudioBuffer<float> dryCopy;

    double sr = 44100.0;
    float prevIn = 1.0f, prevOut = 1.0f;
    int currentPreset = 0;

    // Estado de la correccion de tono
    float correction = 0.0f, desired = 0.0f, lastF0 = 0.0f;
    bool voiced = false;
    int unvoicedSamples = 0;

    // Asistente
    std::atomic<int> assistState { assistIdle };
    std::atomic<float> assistProgress { 0.0f };
    std::atomic<bool> assistResetRequest { false };
    std::atomic<float> assistResult[4] { 0.0f, 0.0f, 0.0f, 0.0f };   // gain offset, comp, deEss, (reservado)
    double aSumSq = 0.0, aSumHigh = 0.0;
    float aPeak = 0.0f;
    long long aCount = 0;
    vx::StereoBiquad assistHp;
    std::vector<std::pair<juce::String, float>> assistBefore, assistAfter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoxoraProcessor)
};
