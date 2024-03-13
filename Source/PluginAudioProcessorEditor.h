#pragma once

#include "PluginAudioProcessor.h"
#include "Components/PresetComponent.h"
#include "PluginLookAndFeel.h"
#include "Components/PreAmpComponent.h"
#include "Components/AmpComponent.h"
#include "Components/CabinetComponent.h"
#include "Components/MixerComponent.h"
#include "Components/TopComponent.h"

class PluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginAudioProcessorEditor (PluginAudioProcessor&);
    ~PluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void launchAsyncFileChooserForImpulseResponse();

private:
    PluginAudioProcessor& mProcessorRef;

    PluginLookAndFeel mLookAndFeel;

    std::unique_ptr<juce::FileChooser> mFileChooser;

    juce::AudioProcessorValueTreeState& mAudioProcessorValueTreeState;

    std::unique_ptr<PresetComponent> mPresetComponentPtr;
    std::unique_ptr<TopComponent> mTopComponent;

    std::unique_ptr<juce::TabbedComponent> mTabbedComponentPtr;
    std::unique_ptr<PreAmpComponent> mPedalsComponentPtr;
    std::unique_ptr<AmpComponent> mAmpComponentPtr;
    std::unique_ptr<CabinetComponent> mCabinetComponentPtr;
    std::unique_ptr<ApvtsIdComponent> mMixerApvtsIdComponentPtr;
    std::unique_ptr<ApvtsIdComponent> mHiddenApvtsIdComponentPtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};
