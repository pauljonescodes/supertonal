cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

project(Blueprint VERSION 0.0.1)

add_subdirectory(modules)

# this should be better
if(APPLE)
    juce_set_aax_sdk_path("$ENV{HOME}/SDKs/AAX")
elseif(WIN32)
    juce_set_aax_sdk_path("C:/SDKs/AAX/")
endif()

if(BUILD_RELEASE)
    set(HARDENED_RUNTIME_ENABLED YES)
else()
    set(HARDENED_RUNTIME_ENABLED NO)
endif()

juce_add_plugin(Blueprint
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME SupertonalDSP                          # Specify the name of the plugin's author
    IS_SYNTH FALSE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD FALSE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Sdsp               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Bpcb                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone AAX              # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "Blueprint Cory Bergeron"     # The name of the final executable, which can differ from the target name
    
    VST3_CATEGORIES Fx Distortion
    AU_MAIN_TYPE kAudioUnitType_Effect
    AAX_CATEGORY AAX_ePlugInCategory_Harmonic
    AAX_IDENTIFIER "com.supertonaldsp.blueprintcb.aax"
    HARDENED_RUNTIME_ENABLED ${HARDENED_RUNTIME_ENABLED}
    HARDENED_RUNTIME_OPTIONS "com.apple.security.device.audio-input"
    AU_SANDBOX_SAFE TRUE
    APP_SANDBOX_ENABLED TRUE
    )        

juce_generate_juce_header(Blueprint)

target_sources(Blueprint
    PRIVATE

        Source/Components/PresetComponent.h 
        Source/Components/AmpComponent.h 
        Source/Components/CabinetComponent.h 
        Source/Components/MainComponent.cpp 
        Source/Components/MixerComponent.h 
        Source/Components/PreAmpComponent.h

        Source/Processors/CircuitQuantityHelper.h
        Source/Processors/CircuitQuantityHelper.cpp
        Source/Processors/MouseDrive.cpp
        Source/Processors/MouseDrive.h
        Source/Processors/MouseDriveWDF.h
        Source/Processors/OmegaProvider.h
        Source/Processors/TubeScreamer.cpp
        Source/Processors/TubeScreamer.h
        Source/Processors/TubeScreamerWDF.h

        Source/PluginUtils.h 
        Source/PluginAudioParameters.h 
        Source/PluginAudioProcessor.cpp 
        Source/PluginAudioProcessor.h 
        Source/PluginAudioProcessorEditor.cpp 
        Source/PluginAudioProcessorEditor.h 
        Source/PluginLookAndFeel.cpp 
        Source/PluginLookAndFeel.h 
        Source/PluginPresetManager.cpp 
        Source/PluginPresetManager.h 
        Source/PluginUtils.cpp)

target_compile_definitions(Blueprint
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

juce_add_binary_data(AudioPluginData SOURCES 
    Assets/VTM60_low_pregain70_low50_mid50_high50_presence50.wav 
    Assets/WorkSans-Bold.ttf 
    Assets/WorkSans-BoldItalic.ttf 
    Assets/WorkSans-Italic.ttf 
    Assets/WorkSans-Light.ttf 
    Assets/WorkSans-LightItalic.ttf 
    Assets/WorkSans-Regular.ttf 
    Assets/cory_bread_and_butter_normalized.wav
)

target_link_libraries(Blueprint
    PRIVATE
        AudioPluginData 
        juce::juce_audio_utils
        juce::juce_dsp
        chowdsp::chowdsp_wdf
        math_approx
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
