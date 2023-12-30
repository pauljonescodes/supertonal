#pragma once

#include "PluginAudioProcessor.h"
#include "PluginPresetComponent.h"

//==============================================================================
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

    juce::AudioProcessorValueTreeState& mAudioProcessorValueTreeState;

    std::unique_ptr<PluginPresetComponent> mPresetComponentPtr;
    std::unique_ptr <juce::Viewport> mViewportPtr;
    std::unique_ptr <juce::Component> mContainerPtr;

    juce::OwnedArray<juce::OwnedArray<juce::Component>> mComponents;

    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> mButtonAttachments;    
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> mSliderAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};
