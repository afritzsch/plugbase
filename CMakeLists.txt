cmake_minimum_required(VERSION 3.22)

# TODO: rename MyPlugin everywhere in this file and in Source/
project(MyPlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# ── JUCE ──────────────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.7
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ── Plugin target ─────────────────────────────────────────────────────────────
juce_add_plugin(MyPlugin
    COMPANY_NAME            "MyPlugins"        # TODO
    PLUGIN_MANUFACTURER_CODE MPlg              # TODO: unique 4-char code
    PLUGIN_CODE             Mypg              # TODO: unique 4-char code
    FORMATS                 VST3 AU
    PRODUCT_NAME            "My Plugin"        # TODO
    IS_SYNTH                FALSE
    NEEDS_MIDI_INPUT        FALSE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES         "Fx"               # TODO
    AU_MAIN_TYPE            kAudioUnitType_Effect
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(MyPlugin)

target_sources(MyPlugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(MyPlugin PUBLIC
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MyPlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_extra
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# ── UI: dev server vs embedded build ─────────────────────────────────────────
option(PLUGIN_DEV_SERVER "Point plugin UI at Vite dev server (localhost:5173)" OFF)

if(PLUGIN_DEV_SERVER)
    message(STATUS "UI: dev server mode — run 'npm run dev' in ui/")
    target_compile_definitions(MyPlugin PRIVATE PLUGIN_UI_DEV=1)
else()
    set(UI_DIST "${CMAKE_CURRENT_SOURCE_DIR}/ui/dist")
    if(NOT EXISTS "${UI_DIST}/index.html")
        message(FATAL_ERROR
            "UI dist not found. Build it first:\n"
            "  cd ui && npm install && npm run build")
    endif()
    file(GLOB UI_DIST_FILES "${UI_DIST}/*")
    juce_add_binary_data(MyPluginUI SOURCES ${UI_DIST_FILES})
    target_link_libraries(MyPlugin PRIVATE MyPluginUI)
    target_compile_definitions(MyPlugin PRIVATE PLUGIN_UI_DEV=0)
    message(STATUS "UI: embedded from ui/dist/")
endif()

# ── Ad-hoc sign after build ───────────────────────────────────────────────────
add_custom_command(TARGET MyPlugin_VST3 POST_BUILD
    COMMAND codesign --force --deep --sign - "$<TARGET_BUNDLE_DIR:MyPlugin_VST3>"
    COMMENT "Ad-hoc signing VST3"
)
add_custom_command(TARGET MyPlugin_AU POST_BUILD
    COMMAND codesign --force --deep --sign - "$<TARGET_BUNDLE_DIR:MyPlugin_AU>"
    COMMENT "Ad-hoc signing AU"
)
