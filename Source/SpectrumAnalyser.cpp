#include "SpectrumAnalyser.h"

void SpectrumAnalyser::pushBlock(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int i = 0; i < numSamples; ++i) {
        float mono = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            mono += buffer.getReadPointer(ch)[i];
        mono /= static_cast<float>(numChannels);

        fftBuffer[static_cast<size_t>(fftBufferIndex)] = mono;
        if (++fftBufferIndex == fftSize) {
            fftBufferIndex = 0;
            std::fill(fftBuffer.begin() + fftSize, fftBuffer.end(), 0.0f);
            fft.performFrequencyOnlyForwardTransform(fftBuffer.data());

            juce::SpinLock::ScopedLockType sl(lock);
            for (int bin = 0; bin < numBins; ++bin) {
                float mag = fftBuffer[static_cast<size_t>(bin)] / static_cast<float>(fftSize);
                bins[static_cast<size_t>(bin)] = juce::Decibels::gainToDecibels(mag, -100.0f);
            }
        }
    }
}

bool SpectrumAnalyser::getMagnitudesDb(std::array<float, numBins>& out)
{
    juce::SpinLock::ScopedTryLockType sl(lock);
    if (!sl.isLocked())
        return false;

    out = bins;
    return true;
}
