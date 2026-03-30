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
