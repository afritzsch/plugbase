#pragma once
#include <JuceHeader.h>
#include "WaveformAnalyser.h"
#include "SpectrumAnalyser.h"

// Owns the 30 Hz timer that pushes analyser data to the browser as events.
class VisualizerEmitter : private juce::Timer
{
public:
    VisualizerEmitter(WaveformAnalyser& waveform, SpectrumAnalyser& spectrum,
                       juce::WebBrowserComponent& browser);

private:
    void timerCallback() override;
    void emitWaveform();
    void emitSpectrum();

    WaveformAnalyser& waveformAnalyser;
    SpectrumAnalyser& spectrumAnalyser;
    juce::WebBrowserComponent& webComponent;
};
