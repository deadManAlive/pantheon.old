cmake_minimum_required(VERSION 3.15)
project(PANTHEON_ADVANCED_PANNER VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

juce_add_plugin(pantheon
    COMPANY_NAME "Twilight's Sound"
    COMPANY_COPYRIGHT "GPLv3"
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "D:/null_/DSP/JUCE/VSTVault"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Pth3
    FORMATS VST3
    PRODUCT_NAME "Pantheon"
    VST3_CATEGORIES FX
)

target_sources(pantheon
    PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
)


target_compile_definitions(pantheon
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(pantheon
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
