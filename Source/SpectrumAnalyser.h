#pragma once
#include <JuceHeader.h>

// pushBlock() runs on the audio thread, getMagnitudesDb() on the message
// thread (never blocks the audio thread, uses a try-lock).
class SpectrumAnalyser
{
public:
    static constexpr int fftOrder = 10;
    static constexpr int fftSize  = 1 << fftOrder;
    static constexpr int numBins  = fftSize / 2;

    void pushBlock(const juce::AudioBuffer<float>& buffer);
    bool getMagnitudesDb(std::array<float, numBins>& out);

private:
    juce::dsp::FFT fft { fftOrder };
    std::array<float, fftSize * 2> fftBuffer {};
    int fftBufferIndex { 0 };

    juce::SpinLock lock;
    std::array<float, numBins> bins {};
};
