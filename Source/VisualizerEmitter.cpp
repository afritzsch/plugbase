#include "VisualizerEmitter.h"

namespace {
    juce::Array<juce::var> downsample(const float* data, size_t len, size_t outCount)
    {
        juce::Array<juce::var> out;
        for (size_t i = 0; i < outCount; ++i)
            out.add((double)data[i * len / outCount]);
        return out;
    }

    void emitSamples(juce::WebBrowserComponent& browser, const char* eventId,
                      const char* propertyName, const float* data, size_t len, size_t outCount)
    {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty(propertyName, juce::var(downsample(data, len, outCount)));
        browser.emitEventIfBrowserIsVisible(eventId, juce::var(obj.get()));
    }
}

VisualizerEmitter::VisualizerEmitter(WaveformAnalyser& waveform, SpectrumAnalyser& spectrum,
                                      juce::WebBrowserComponent& browser)
    : waveformAnalyser(waveform), spectrumAnalyser(spectrum), webComponent(browser)
{
    startTimerHz(30);
}

void VisualizerEmitter::timerCallback()
{
    emitWaveform();
    emitSpectrum();
}

void VisualizerEmitter::emitWaveform()
{
    auto samples = waveformAnalyser.readLatest(256);
    if (samples.empty()) return;

    emitSamples(webComponent, "waveformUpdate", "samples", samples.data(), samples.size(), 256);
}

void VisualizerEmitter::emitSpectrum()
{
    std::array<float, SpectrumAnalyser::numBins> mags;
    if (!spectrumAnalyser.getMagnitudesDb(mags)) return;

    emitSamples(webComponent, "spectrumUpdate", "bins", mags.data(), mags.size(), 128);
}
