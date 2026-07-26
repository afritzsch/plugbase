#pragma once
#include <JuceHeader.h>

// pushBlock() runs on the audio thread, readLatest() on the message thread.
class WaveformAnalyser
{
public:
    static constexpr int bufferSize = 2048;

    void pushBlock(const juce::AudioBuffer<float>& buffer);
    std::vector<float> readLatest(int minSamples);

private:
    juce::AbstractFifo fifo { bufferSize };
    std::array<float, bufferSize> buffer {};
};
