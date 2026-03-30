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

void MyPluginEditor::emitWaveform()
{
    const int available = proc.waveformFifo.getNumReady();
    if (available < 256) return;

    std::vector<float> temp(static_cast<size_t>(available));
    int start1, size1, start2, size2;
    proc.waveformFifo.prepareToRead(available, start1, size1, start2, size2);
    std::copy(proc.waveformBuffer.begin() + start1,
              proc.waveformBuffer.begin() + start1 + size1,
              temp.begin());
    std::copy(proc.waveformBuffer.begin() + start2,
              proc.waveformBuffer.begin() + start2 + size2,
              temp.begin() + size1);
    proc.waveformFifo.finishedRead(size1 + size2);

    constexpr int outSamples = 256;
    juce::Array<juce::var> pts;
    for (int i = 0; i < outSamples; ++i) {
        int idx = (int)(i * (float)temp.size() / outSamples);
        pts.add((double)temp[static_cast<size_t>(idx)]);
    }
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty("samples", juce::var(pts));
    webComponent.emitEventIfBrowserIsVisible("waveformUpdate", juce::var(obj.get()));
}

void MyPluginEditor::emitSpectrum()
{
    juce::SpinLock::ScopedTryLockType lock(proc.spectrumLock);
    if (!lock.isLocked()) return;

    constexpr int outBins = 128;
    juce::Array<juce::var> bins;
    for (int i = 0; i < outBins; ++i) {
        int idx = i * (MyPluginProcessor::fftSize / 2) / outBins;
        bins.add((double)proc.spectrumBins[idx]);
    }
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty("bins", juce::var(bins));
    webComponent.emitEventIfBrowserIsVisible("spectrumUpdate", juce::var(obj.get()));
}
