# plugbase

Starter template for JUCE audio plugins with a React/TypeScript web UI powered by [plugui](https://github.com/afritzsch/plugui).

## Starting a new plugin

**1. Clone this template**
```sh
git clone https://github.com/afritzsch/plugbase.git MyPlugin
cd MyPlugin
rm -rf .git && git init
```

**2. Add the plugui submodule**
```sh
git submodule add https://github.com/afritzsch/plugui.git ui/plugui
```

**3. Rename `MyPlugin` to your plugin name**

Find and replace `MyPlugin` in:
- `CMakeLists.txt` — target name, binary data target, codesign targets
- `Source/PluginProcessor.h` / `.cpp` — class name
- `Source/PluginEditor.h` / `.cpp` — class name
- `ui/package.json` — `name` field
- `ui/index.html` — `<title>`
- `ui/src/App.tsx` — plugin title text

Also update in `CMakeLists.txt`:
- `COMPANY_NAME`, `PLUGIN_MANUFACTURER_CODE`, `PLUGIN_CODE`, `PRODUCT_NAME`

**4. Build the UI**
```sh
cd ui && npm install && npm run build && cd ..
```

**5. Build the plugin**
```sh
mkdir build && cd build
cmake -G Ninja ..
ninja MyPlugin
```

---

## Adding a parameter

**C++ — `PluginProcessor.cpp`** (`createParameterLayout`):
```cpp
layout.add(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{ "gain", 1 }, "Gain",
    juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f), 0.0f,
    juce::AudioParameterFloatAttributes().withLabel("dB")
));
```

**C++ — `PluginEditor.h`** (add relay + attachment):
```cpp
juce::WebSliderRelay gainRelay { "gainSlider" };
juce::WebSliderParameterAttachment gainAttachment;
```

**C++ — `PluginEditor.cpp`** (wire in constructor):
```cpp
webComponent(juce::WebBrowserComponent::Options{}
    .withNativeIntegrationEnabled()
    .withOptionsFrom(gainRelay)   // add this
    ...),
gainAttachment(*dynamic_cast<juce::RangedAudioParameter*>(
    proc.apvts.getParameter("gain")), gainRelay)
```

**JS — `ui/src/juceDevMock.ts`** (mock for browser dev):
```ts
__juce__sliders: ['gainSlider'],
// and in emitEvent, respond with properties + initial value
```

**JS — `ui/src/App.tsx`**:
```tsx
import { Knob, useJuceSlider } from 'plugui'

const gain = useJuceSlider('gainSlider')
<Knob value={gain.scaledValue} min={gain.properties.start} max={gain.properties.end} ... />
```

---

## Development (live reload)

```sh
cd ui && npm run dev
```

```sh
cmake -G Ninja -DPLUGIN_DEV_SERVER=ON ..
ninja MyPlugin
```
