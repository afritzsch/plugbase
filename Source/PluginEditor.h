#pragma once
#include "PluginProcessor.h"

class MyPluginEditor : public juce::AudioProcessorEditor,
                       private juce::Timer
{
public:
    explicit MyPluginEditor(MyPluginProcessor&);
    ~MyPluginEditor() override = default;

    void paint(juce::Graphics&) override {}
    void resized() override;

private:
    static std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    MyPluginProcessor& proc;

    // TODO: add WebSliderRelay / WebToggleButtonRelay for each parameter, e.g.:
    // juce::WebSliderRelay gainRelay { "gainSlider" };

    struct Browser : juce::WebBrowserComponent
    {
        using WebBrowserComponent::WebBrowserComponent;
        bool pageAboutToLoad(const juce::String& url) override
        {
            return url.startsWith("http://localhost:5173")
                || url == getResourceProviderRoot();
        }
    };

    Browser webComponent;

    // TODO: add WebSliderParameterAttachment / WebToggleButtonParameterAttachment, e.g.:
    // juce::WebSliderParameterAttachment gainAttachment;

    void timerCallback() override;

    // ── Visualizers ── (remove if not needed) ─────────────────────────────────
    void emitWaveform();
    void emitSpectrum();
    // ──────────────────────────────────────────────────────────────────────────

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyPluginEditor)
};
