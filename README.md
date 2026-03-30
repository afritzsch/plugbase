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

**3. Rename to your plugin name**
```sh
chmod +x rename.sh && ./rename.sh SimpleReverb
```

Then update the remaining fields in `CMakeLists.txt`:
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


## Development (live reload)

```sh
cd ui && npm run dev
```

```sh
cmake -G Ninja -DPLUGIN_DEV_SERVER=ON ..
ninja MyPlugin
```
