#pragma once

#include "PluginAudioProcessor.h"
#include "Components/PresetComponent.h"
#include "PluginLookAndFeel.h"
#include "Components/PreAmpComponent.h"
#include "Components/AmpComponent.h"
#include "Components/CabinetComponent.h"
#include "Components/MixerComponent.h"

class PluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginAudioProcessorEditor (
        PluginAudioProcessor&, 
        juce::AudioProcessorValueTreeState&,
        PluginPresetManager& presetManager);
    ~PluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginAudioProcessor& mProcessorRef;

    PluginLookAndFeel mLookAndFeel;

    juce::AudioProcessorValueTreeState& mAudioProcessorValueTreeState;

    std::unique_ptr<PresetComponent> mPresetComponentPtr;

    std::unique_ptr<juce::TabbedComponent> mTabbedComponentPtr;
    std::unique_ptr<PedalsComponent> mPedalsComponentPtr;
    std::unique_ptr<AmpComponent> mAmpComponentPtr;
    std::unique_ptr<CabinetComponent> mCabinetComponentPtr;
    std::unique_ptr<MixerComponent> mMixerComponentPtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};
