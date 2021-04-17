juce_add_console_app(Commandline
        COMPANY_NAME Ultraschall
        COMPANY_WEBSITE https://ultraschall.fm
        COMPANY_EMAIL info@ultraschall.fm
        DESCRIPTION Ultraschall Soundboard Server
        PRODUCT_NAME "Soundboard Server"
        )

target_compile_definitions(Commandline PUBLIC JUCER_ENABLE_GPL_MODE=1)
target_compile_definitions(Commandline PUBLIC JUCE_DISPLAY_SPLASH_SCREEN=0)
target_compile_definitions(Commandline PUBLIC JUCE_USE_LAME_AUDIO_FORMAT=1)

set(SOURCE_LIST
    Main.cpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Plugin/Source Files" FILES ${SOURCE_LIST})

target_sources(Commandline PUBLIC
    ${SOURCE_LIST}
    ${HEADER_LIST}
)

target_compile_definitions(Commandline
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)

target_link_libraries(Commandline
        PUBLIC
        juce::juce_core
        juce::juce_osc
        juce::juce_audio_formats
        juce::juce_audio_utils
        juce::juce_audio_devices
        ultraschall::ultraschall_cli
        ultraschall::ultraschall_audio
        ultraschall::ultraschall_store
        PRIVATE
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
