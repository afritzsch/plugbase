#pragma once
#include <JuceHeader.h>

class MyPluginProcessor : public juce::AudioProcessor
{
public:
    MyPluginProcessor();
    ~MyPluginProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // ── Visualizers ── (remove if not needed) ─────────────────────────────────
    static constexpr int waveformSize = 2048;
    juce::AbstractFifo              waveformFifo   { waveformSize };
    std::array<float, waveformSize> waveformBuffer {};

    static constexpr int fftOrder = 10;
    static constexpr int fftSize  = 1 << fftOrder;
    juce::dsp::FFT                 fft            { fftOrder };
    std::array<float, fftSize * 2> fftBuffer      {};
    int                            fftBufferIndex { 0 };
    juce::SpinLock                 spectrumLock;
    std::array<float, fftSize / 2> spectrumBins   {};
    // ──────────────────────────────────────────────────────────────────────────

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // ── Visualizers ── (remove if not needed) ─────────────────────────────────
    void updateWaveform(const juce::AudioBuffer<float>& buffer);
    void updateSpectrum(const juce::AudioBuffer<float>& buffer);
    // ──────────────────────────────────────────────────────────────────────────

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyPluginProcessor)
};
