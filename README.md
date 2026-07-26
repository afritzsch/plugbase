# plugbase

A starter template for JUCE audio plugins (VST3/AU) that swaps the usual hand-painted `juce::Graphics` UI for a real React/TypeScript web app, rendered inside a `juce::WebBrowserComponent`. Parameters stay wired up to the host through JUCE's native web relay and a standard `AudioProcessorValueTreeState`, so automation and state save/load work exactly like any other JUCE plugin. You just get to build the UI with CSS instead of paint calls.

The UI layer leans on [plugui](https://github.com/afritzsch/plugui), a small component library (knobs, sliders, meters, waveform/spectrum visualizers) built for this JUCE/web setup.

## What you get out of the box

- A working, buildable VST3/AU plugin with an empty parameter layout. Add your DSP and your params, everything else is already wired.
- A React UI with hot reload during development (Vite dev server) and a binary-embedded production build.
- Two-way parameter binding (slider/toggle relays + attachments), so host automation and UI stay in sync.
- Optional waveform and spectrum visualizers, pushed from the audio thread to the UI at 30 Hz. Fenced off in the code so you can delete them in one pass if you don't need them.
- A browser-mode mock (`juceDevMock.ts`) so the UI runs and is developable in a plain browser tab, no JUCE host needed.

For the deeper architectural notes (how the C++/web bridge works, the audio-thread to UI handoff, the Vite build constraints) see `CLAUDE.md`.

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

**3. Rename to your plugin name**
```sh
./rename.sh SimpleReverb
```
This rewrites the `MyPlugin` / `My Plugin` / `my-plugin-ui` placeholders across `CMakeLists.txt`, `Source/` and the `ui/` files. It won't catch everything though, so afterwards go fill in the remaining fields in `CMakeLists.txt` by hand:
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

The order here matters. CMake fetches JUCE on first configure, and `ui/package.json` points at a JUCE source file that only exists after that, so `cmake` has to run before `npm install`. On the other end, the plugin build hard-fails if `ui/dist/index.html` is missing, so the UI has to be built before `ninja`.

`COPY_PLUGIN_AFTER_BUILD` is on by default, so a successful build drops the VST3/AU straight into your system plugin folders (and ad-hoc codesigns both on macOS).

## Development (live reload)

Run both halves side by side: the Vite dev server for the UI, and the plugin pointed at it instead of the embedded build.

```sh
cd ui && npm run dev
```

```sh
cmake -G Ninja -DPLUGIN_DEV_SERVER=ON ..
ninja MyPlugin
```

Edit `App.tsx`, save, and the plugin window updates without a rebuild.

## Requirements

- CMake 3.22+ and a generator (Ninja recommended)
- Node.js / npm
- macOS for AU support and the codesign step. VST3-only builds should work on other platforms but this template hasn't been tested there.
