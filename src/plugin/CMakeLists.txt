juce_add_plugin(Plugin
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Ultr
        PLUGIN_CODE Sndb
        COMPANY_NAME Ultraschall
        COMPANY_WEBSITE https://ultraschall.fm
        COMPANY_EMAIL info@ultraschall.fm
        DESCRIPTION Ultraschall Soundboard
        FORMATS AU VST3 LV2 Standalone
        VST3_CATEGORIES Instrument
        AU_MAIN_TYPE kAudioUnitType_MusicDevice
        PRODUCT_NAME "Ultraschall Soundboard"
        )

target_compile_definitions(Plugin PUBLIC JUCER_ENABLE_GPL_MODE=1)
target_compile_definitions(Plugin PUBLIC JUCE_DISPLAY_SPLASH_SCREEN=0)
target_compile_definitions(Plugin PUBLIC JUCE_USE_LAME_AUDIO_FORMAT=1)

set(SOURCE_LIST
    PluginProcessor.cpp
    PluginEditor.cpp
    PlayerView.cpp
)

set(HEADER_LIST
    PluginProcessor.h
    PluginEditor.h
    PlayerView.h
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Plugin/Source Files" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Plugin/Header Files" FILES ${HEADER_LIST})

target_sources(Plugin PUBLIC
    ${SOURCE_LIST}
    ${HEADER_LIST}
)

target_compile_definitions(Plugin
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Plugin
        PUBLIC
        juce::juce_audio_utils
        juce::juce_audio_formats
        juce::juce_audio_utils
        ultraschall::ultraschall_audio
        ultraschall::ultraschall_store
        PRIVATE
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
