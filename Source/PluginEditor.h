#pragma once

#include "Constants.h"
#include "PluginProcessor.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& mProcessorRef;

    juce::Slider mInputGainSlider;
    juce::Label mInputGainLabel { constants::inputGainParameterId + "-label", constants::inputGainParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mInputGainSliderAttachment;

    juce::Slider mOverdriveBiasSlider;
    juce::Label mOverdriveBiasLabel { constants::overdriveBiasParameterId + "-label", constants::overdriveBiasParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOverdriveBiasSliderAttachment;

    juce::Slider mOverdriveGainSlider;
    juce::Label mOverdriveGainLabel { constants::overdriveGainParameterId + "-label", constants::overdriveGainParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOverdriveGainSliderAttachment;

    juce::ToggleButton mOverdriveTanhBypassToggleButton { constants::overdriveTanhWaveShaperBypassParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mOverdriveTanhBypassToggleButtonAttachment;

    juce::ToggleButton mOverdriveSoftClipBypassToggleButton { constants::overdriveSoftClipWaveShaperBypassParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mOverdriveSoftClipBypassToggleButtonAttachment;

    //

    juce::Slider mPowerAmpLowShelfFrequencySlider;
    juce::Label mPowerAmpLowShelfFrequencyLabel { constants::powerAmpLowShelfFrequencyParameterId + "-label", constants::powerAmpLowShelfFrequencyParamName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpLowShelfFrequencySliderAttachment;

    juce::Slider mPowerAmpLowShelfQualitySlider;
    juce::Label mPowerAmpLowShelfQualityLabel { constants::powerAmpLowShelfQualityParameterId + "-label", constants::powerAmpLowShelfQualityParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpLowShelfQualitySliderAttachment;

    juce::Slider mPowerAmpLowShelfGainSlider;
    juce::Label mPowerAmpLowShelfGainLabel { constants::powerAmpLowShelfGainParameterId + "-label", constants::powerAmpLowShelfGainParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpLowShelfGainSliderAttachment;

    //

    juce::Slider mPowerAmpPeakFilterFrequencySlider;
    juce::Label mPowerAmpPeakFilterFrequencyLabel { constants::powerAmpPeakFilterFrequencyParameterId + "-label", constants::powerAmpPeakFilterFrequencyParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpPeakFilterFrequencySliderAttachment;

    juce::Slider mPowerAmpPeakFilterQualitySlider;
    juce::Label mPowerAmpPeakFilterQualityLabel { constants::powerAmpPeakFilterQualityParameterId + "-label", constants::powerAmpPeakFilterQualityParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpPeakFilterQualitySliderAttachment;

    juce::Slider mPowerAmpPeakFilterGainSlider;
    juce::Label mPowerAmpPeakFilterGainLabel { constants::powerAmpPeakFilterGainParameterId + "-label", constants::powerAmpPeakFilterGainParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpPeakFilterGainSliderAttachment;

    //

    juce::Slider mPowerAmpHighShelfFrequencySlider;
    juce::Label mPowerAmpHighShelfFrequencyLabel { constants::powerAmpHighShelfFrequencyParameterId + "-label", constants::powerAmpHighShelfFrequencyParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpHighShelfFrequencySliderAttachment;

    juce::Slider mPowerAmpHighShelfQualitySlider;
    juce::Label mPowerAmpHighShelfQualityLabel { constants::powerAmpHighShelfQualityParameterId + "-label", constants::powerAmpHighShelfQualityParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpHighShelfQualitySliderAttachment;

    juce::Slider mPowerAmpHighShelfGainSlider;
    juce::Label mPowerAmpHighShelfGainLabel { constants::powerAmpHighShelfGainParameterId + "-label", constants::powerAmpHighShelfGainParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mPowerAmpHighShelfGainSliderAttachment;

    //
    juce::Slider mReverbBlendSlider;
    juce::Label mReverbBlendLabel { constants::reverbBlendParameterId + "-label", constants::reverbBlendParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mReverbBlendSliderAttachment;

    //

    juce::Slider mOutputGainSlider;
    juce::Label mOutputGainLabel { constants::outputGainParameterId + "-label", constants::outputGainParameterName };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOutputGainSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
