#include "WaveformAnalyser.h"

void WaveformAnalyser::pushBlock(const juce::AudioBuffer<float>& audio)
{
    const int numSamples  = audio.getNumSamples();
    const int numChannels = audio.getNumChannels();

    int start1, size1, start2, size2;
    fifo.prepareToWrite(numSamples, start1, size1, start2, size2);

    auto writeRange = [&](int destStart, int srcOffset, int count) {
        for (int i = 0; i < count; ++i) {
            float mono = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
                mono += audio.getReadPointer(ch)[srcOffset + i];
            buffer[static_cast<size_t>(destStart + i)] = mono / static_cast<float>(numChannels);
        }
    };

    writeRange(start1, 0, size1);
    writeRange(start2, size1, size2);

    fifo.finishedWrite(size1 + size2);
}

std::vector<float> WaveformAnalyser::readLatest(int minSamples)
{
    const int available = fifo.getNumReady();
    if (available < minSamples)
        return {};

    std::vector<float> out(static_cast<size_t>(available));
    int start1, size1, start2, size2;
    fifo.prepareToRead(available, start1, size1, start2, size2);

    std::copy(buffer.begin() + start1, buffer.begin() + start1 + size1, out.begin());
    std::copy(buffer.begin() + start2, buffer.begin() + start2 + size2, out.begin() + size1);

    fifo.finishedRead(size1 + size2);
    return out;
}
