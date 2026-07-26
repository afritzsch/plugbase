#include "PluginEditor.h"

#if !PLUGIN_UI_DEV
  #include <BinaryData.h>
#endif

MyPluginEditor::MyPluginEditor(MyPluginProcessor& p)
    : AudioProcessorEditor(&p),
      proc(p),
      webComponent(juce::WebBrowserComponent::Options{}
          .withNativeIntegrationEnabled()
          // TODO: add .withOptionsFrom(yourRelay) for each relay
          .withResourceProvider([](const juce::String& url) { return getResource(url); }))
      // TODO: initialise attachments here, e.g.:
      // gainAttachment(*dynamic_cast<juce::RangedAudioParameter*>(proc.apvts.getParameter("gain")), gainRelay)
{
    addAndMakeVisible(webComponent);

#if PLUGIN_UI_DEV
    webComponent.goToURL("http://localhost:5173/");
#else
    webComponent.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#endif

    setSize(400, 300); // TODO: set your plugin window size
    startTimerHz(30);
}

void MyPluginEditor::timerCallback()
{
    emitWaveform(); // remove if not needed
    emitSpectrum(); // remove if not needed
}

void MyPluginEditor::resized()
{
    webComponent.setBounds(getLocalBounds());
}

std::optional<juce::WebBrowserComponent::Resource>
MyPluginEditor::getResource(const juce::String& url)
{
#if PLUGIN_UI_DEV
    juce::ignoreUnused(url);
    return std::nullopt;
#else
    auto filename = (url == "/" || url == "/index.html") ? "index.html"
                  : url.trimCharactersAtStart("/");

    juce::String resourceName;
    for (auto c : filename)
        resourceName += (juce::CharacterFunctions::isLetterOrDigit(c) ? c : juce::juce_wchar('_'));

    int dataSize = 0;
    auto* data = BinaryData::getNamedResource(resourceName.toRawUTF8(), dataSize);

    if (data == nullptr)
        return std::nullopt;

    const auto mimeType = [&]() -> juce::String {
        if (filename.endsWith(".html")) return "text/html";
        if (filename.endsWith(".js"))   return "text/javascript";
        if (filename.endsWith(".css"))  return "text/css";
        if (filename.endsWith(".svg"))  return "image/svg+xml";
        if (filename.endsWith(".png"))  return "image/png";
        return "application/octet-stream";
    }();

    std::vector<std::byte> bytes(static_cast<size_t>(dataSize));
    std::memcpy(bytes.data(), data, static_cast<size_t>(dataSize));
    return juce::WebBrowserComponent::Resource{ std::move(bytes), mimeType };
#endif
}

// ── Visualizers ── (remove if not needed) ─────────────────────────────────────

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

void MyPluginEditor::emitWaveform()
{
    auto samples = proc.waveformAnalyser.readLatest(256);
    if (samples.empty()) return;

    emitSamples(webComponent, "waveformUpdate", "samples", samples.data(), samples.size(), 256);
}

void MyPluginEditor::emitSpectrum()
{
    std::array<float, SpectrumAnalyser::numBins> mags;
    if (!proc.spectrumAnalyser.getMagnitudesDb(mags)) return;

    emitSamples(webComponent, "spectrumUpdate", "bins", mags.data(), mags.size(), 128);
}
