#include "PluginEditor.h"
#include "Constants.h"

using namespace juce;
using namespace std;
using namespace constants;

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), mProcessorRef (p)
{
    auto textBoxWidth = 75;
    auto textBoxHeight = 15;

    // INPUT GAIN

    mInputGainSlider.setSliderStyle (Slider::Rotary);
    mInputGainSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mInputGainSlider.setRange (gainMinimumValue, gainMaximumValue, gainIntervalValue);
    mInputGainSlider.setTitle (inputGainParameterName);
    mInputGainSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        inputGainParameterId,
        mInputGainSlider);
    addAndMakeVisible (mInputGainSlider);

    addAndMakeVisible (mInputGainLabel);
    mInputGainLabel.attachToComponent (&mInputGainSlider, false);

    // OD BIAS

    mOverdriveBiasSlider.setSliderStyle (Slider::Rotary);
    mOverdriveBiasSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mOverdriveBiasSlider.setRange (biasMinimumValue, biasMaximumValue, biasIntervalValue);
    mOverdriveBiasSlider.setTitle (overdriveBiasParameterName);
    mOverdriveBiasSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        overdriveBiasParameterId,
        mOverdriveBiasSlider);
    addAndMakeVisible (mOverdriveBiasSlider);

    addAndMakeVisible (mOverdriveBiasLabel);
    mOverdriveBiasLabel.attachToComponent (&mOverdriveBiasSlider, false);

    // OD GAIN

    mOverdriveGainSlider.setSliderStyle (Slider::Rotary);
    mOverdriveGainSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mOverdriveGainSlider.setRange (gainMinimumValue, gainMaximumValue, gainIntervalValue);
    mOverdriveGainSlider.setTitle (overdriveGainParameterName);
    mOverdriveGainSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        overdriveGainParameterId,
        mOverdriveGainSlider);
    addAndMakeVisible (mOverdriveGainSlider);

    addAndMakeVisible (mOverdriveGainLabel);
    mOverdriveGainLabel.attachToComponent (&mOverdriveGainSlider, false);

    // OD TANH BYPASS

    mOverdriveTanhBypassToggleButtonAttachment = make_unique<AudioProcessorValueTreeState::ButtonAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        overdriveTanhWaveShaperBypassParameterId,
        mOverdriveTanhBypassToggleButton);
    addAndMakeVisible (mOverdriveTanhBypassToggleButton);

    // OD SOFT CLIP BYPASS

    mOverdriveSoftClipBypassToggleButtonAttachment = make_unique<AudioProcessorValueTreeState::ButtonAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        overdriveSoftClipWaveShaperBypassParameterId,
        mOverdriveSoftClipBypassToggleButton);
    addAndMakeVisible (mOverdriveSoftClipBypassToggleButton);

    // POWER AMP LOW SHELF

    mPowerAmpLowShelfFrequencySlider.setSliderStyle (Slider::Rotary);
    mPowerAmpLowShelfFrequencySlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpLowShelfFrequencySlider.setRange (frequencyMinimumValue, frequencyMaximumValue, frequencyIntervalValue);
    mPowerAmpLowShelfFrequencySlider.setTitle (powerAmpLowShelfFrequencyParamName);
    mPowerAmpLowShelfFrequencySliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpLowShelfFrequencyParameterId,
        mPowerAmpLowShelfFrequencySlider);
    addAndMakeVisible (mPowerAmpLowShelfFrequencySlider);

    addAndMakeVisible (mPowerAmpLowShelfFrequencyLabel);
    mPowerAmpLowShelfFrequencyLabel.attachToComponent (&mPowerAmpLowShelfFrequencySlider, false);

    mPowerAmpLowShelfQualitySlider.setSliderStyle (Slider::Rotary);
    mPowerAmpLowShelfQualitySlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpLowShelfQualitySlider.setRange (qualityMinimumValue, qualityMaximumValue, qualityIntervalValue);
    mPowerAmpLowShelfQualitySlider.setTitle (powerAmpLowShelfQualityParameterName);
    mPowerAmpLowShelfQualitySliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpLowShelfQualityParameterId,
        mPowerAmpLowShelfQualitySlider);
    addAndMakeVisible (mPowerAmpLowShelfQualitySlider);

    addAndMakeVisible (mPowerAmpLowShelfQualityLabel);
    mPowerAmpLowShelfQualityLabel.attachToComponent (&mPowerAmpLowShelfQualitySlider, false);

    mPowerAmpLowShelfGainSlider.setSliderStyle (Slider::Rotary);
    mPowerAmpLowShelfGainSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpLowShelfGainSlider.setRange (gainMinimumValue, gainMaximumValue, gainIntervalValue);
    mPowerAmpLowShelfGainSlider.setTitle (powerAmpLowShelfGainParameterName);
    mPowerAmpLowShelfGainSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpLowShelfGainParameterId,
        mPowerAmpLowShelfGainSlider);
    addAndMakeVisible (mPowerAmpLowShelfGainSlider);

    addAndMakeVisible (mPowerAmpLowShelfGainLabel);
    mPowerAmpLowShelfGainLabel.attachToComponent (&mPowerAmpLowShelfGainSlider, false);

    // POWER AMP PEAK FILTER

    mPowerAmpPeakFilterFrequencySlider.setSliderStyle (Slider::Rotary);
    mPowerAmpPeakFilterFrequencySlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpPeakFilterFrequencySlider.setRange (frequencyMinimumValue, frequencyMaximumValue, frequencyIntervalValue);
    mPowerAmpPeakFilterFrequencySlider.setTitle (powerAmpPeakFilterFrequencyParameterName);
    mPowerAmpPeakFilterFrequencySliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpPeakFilterFrequencyParameterId,
        mPowerAmpPeakFilterFrequencySlider);
    addAndMakeVisible (mPowerAmpPeakFilterFrequencySlider);

    addAndMakeVisible (mPowerAmpPeakFilterFrequencyLabel);
    mPowerAmpPeakFilterFrequencyLabel.attachToComponent (&mPowerAmpPeakFilterFrequencySlider, false);

    mPowerAmpPeakFilterQualitySlider.setSliderStyle (Slider::Rotary);
    mPowerAmpPeakFilterQualitySlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpPeakFilterQualitySlider.setRange (qualityMinimumValue, qualityMaximumValue, qualityIntervalValue);
    mPowerAmpPeakFilterQualitySlider.setTitle (powerAmpPeakFilterQualityParameterName);
    mPowerAmpPeakFilterQualitySliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpPeakFilterQualityParameterId,
        mPowerAmpPeakFilterQualitySlider);
    addAndMakeVisible (mPowerAmpPeakFilterQualitySlider);

    addAndMakeVisible (mPowerAmpPeakFilterQualityLabel);
    mPowerAmpPeakFilterQualityLabel.attachToComponent (&mPowerAmpPeakFilterQualitySlider, false);

    mPowerAmpPeakFilterGainSlider.setSliderStyle (Slider::Rotary);
    mPowerAmpPeakFilterGainSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpPeakFilterGainSlider.setRange (gainMinimumValue, gainMaximumValue, gainIntervalValue);
    mPowerAmpPeakFilterGainSlider.setTitle (powerAmpPeakFilterGainParameterName);
    mPowerAmpPeakFilterGainSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpPeakFilterGainParameterId,
        mPowerAmpPeakFilterGainSlider);
    addAndMakeVisible (mPowerAmpPeakFilterGainSlider);

    addAndMakeVisible (mPowerAmpPeakFilterGainLabel);
    mPowerAmpPeakFilterGainLabel.attachToComponent (&mPowerAmpPeakFilterGainSlider, false);

    // POWER AMP HIGH SHELF

    mPowerAmpHighShelfFrequencySlider.setSliderStyle (Slider::Rotary);
    mPowerAmpHighShelfFrequencySlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpHighShelfFrequencySlider.setRange (frequencyMinimumValue, frequencyMaximumValue, frequencyIntervalValue);
    mPowerAmpHighShelfFrequencySlider.setTitle (powerAmpHighShelfFrequencyParameterName);
    mPowerAmpHighShelfFrequencySliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpHighShelfFrequencyParameterId,
        mPowerAmpHighShelfFrequencySlider);
    addAndMakeVisible (mPowerAmpHighShelfFrequencySlider);

    addAndMakeVisible (mPowerAmpHighShelfFrequencyLabel);
    mPowerAmpHighShelfFrequencyLabel.attachToComponent (&mPowerAmpHighShelfFrequencySlider, false);

    mPowerAmpHighShelfQualitySlider.setSliderStyle (Slider::Rotary);
    mPowerAmpHighShelfQualitySlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpHighShelfQualitySlider.setRange (qualityMinimumValue, qualityMaximumValue, qualityIntervalValue);
    mPowerAmpHighShelfQualitySlider.setTitle (powerAmpHighShelfQualityParameterName);
    mPowerAmpHighShelfQualitySliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpHighShelfQualityParameterId,
        mPowerAmpHighShelfQualitySlider);
    addAndMakeVisible (mPowerAmpHighShelfQualitySlider);

    addAndMakeVisible (mPowerAmpHighShelfQualityLabel);
    mPowerAmpHighShelfQualityLabel.attachToComponent (&mPowerAmpHighShelfQualitySlider, false);

    mPowerAmpHighShelfGainSlider.setSliderStyle (Slider::Rotary);
    mPowerAmpHighShelfGainSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mPowerAmpHighShelfGainSlider.setRange (gainMinimumValue, gainMaximumValue, gainIntervalValue);
    mPowerAmpHighShelfGainSlider.setTitle (powerAmpHighShelfGainParameterName);
    mPowerAmpHighShelfGainSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        powerAmpHighShelfGainParameterId,
        mPowerAmpHighShelfGainSlider);
    addAndMakeVisible (mPowerAmpHighShelfGainSlider);

    addAndMakeVisible (mPowerAmpHighShelfGainLabel);
    mPowerAmpHighShelfGainLabel.attachToComponent (&mPowerAmpHighShelfGainSlider, false);

    // REVERB BLEND

    mReverbBlendSlider.setSliderStyle (Slider::Rotary);
    mReverbBlendSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mReverbBlendSlider.setRange (blendMinimumValue, blendMaximumValue, blendIntervalValue);
    mReverbBlendSlider.setTitle (reverbBlendParameterName);
    mReverbBlendSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        reverbBlendParameterId,
        mReverbBlendSlider);
    addAndMakeVisible (mReverbBlendSlider);

    addAndMakeVisible (mReverbBlendLabel);
    mReverbBlendLabel.attachToComponent (&mReverbBlendSlider, false);

    // OUTPUT GAIN

    mOutputGainSlider.setSliderStyle (Slider::Rotary);
    mOutputGainSlider.setTextBoxStyle (Slider::TextBoxBelow, false, textBoxWidth, textBoxHeight);
    mOutputGainSlider.setRange (gainMinimumValue, gainMaximumValue, gainIntervalValue);
    mOutputGainSlider.setTitle (outputGainParameterName);
    mOutputGainSliderAttachment = make_unique<AudioProcessorValueTreeState::SliderAttachment> (
        mProcessorRef.mProcessorValueTreeState,
        outputGainParameterId,
        mOutputGainSlider);
    addAndMakeVisible (mOutputGainSlider);

    addAndMakeVisible (mOutputGainLabel);
    mOutputGainLabel.attachToComponent (&mOutputGainSlider, false);

    setSize (650, 750);
    setResizable (true, true);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    auto widthAndHeight = 125;
    auto paddingTop = 25;
    auto padding = 10;
    auto labelHeight = 25;

    //

    mInputGainSlider.setBounds (
        padding + widthAndHeight * 0,
        paddingTop + widthAndHeight * 0,
        widthAndHeight,
        widthAndHeight);

    mOverdriveBiasSlider.setBounds (
        padding + widthAndHeight * 1,
        paddingTop + (labelHeight + widthAndHeight) * 0,
        widthAndHeight,
        widthAndHeight);
    mOverdriveTanhBypassToggleButton.setBounds (
        padding + widthAndHeight * 2,
        paddingTop + (labelHeight + widthAndHeight) * 0,
        widthAndHeight,
        widthAndHeight);
    mOverdriveSoftClipBypassToggleButton.setBounds (
        padding + widthAndHeight * 3,
        paddingTop + (labelHeight + widthAndHeight) * 0,
        widthAndHeight,
        widthAndHeight);
    mOverdriveGainSlider.setBounds (
        padding + widthAndHeight * 4,
        paddingTop + (labelHeight + widthAndHeight) * 0,
        widthAndHeight,
        widthAndHeight);

    //

    mPowerAmpLowShelfFrequencySlider.setBounds (
        padding + widthAndHeight * 0,
        paddingTop + (labelHeight + widthAndHeight) * 1,
        widthAndHeight,
        widthAndHeight);
    mPowerAmpLowShelfQualitySlider.setBounds (
        padding + widthAndHeight * 1,
        paddingTop + (labelHeight + widthAndHeight) * 1,
        widthAndHeight,
        widthAndHeight);
    mPowerAmpLowShelfGainSlider.setBounds (
        padding + widthAndHeight * 2,
        paddingTop + (labelHeight + widthAndHeight) * 1,
        widthAndHeight,
        widthAndHeight);

    //

    mPowerAmpPeakFilterFrequencySlider.setBounds (
        padding + widthAndHeight * 0,
        paddingTop + (labelHeight + widthAndHeight) * 2,
        widthAndHeight,
        widthAndHeight);
    mPowerAmpPeakFilterQualitySlider.setBounds (
        padding + widthAndHeight * 1,
        paddingTop + (labelHeight + widthAndHeight) * 2,
        widthAndHeight,
        widthAndHeight);
    mPowerAmpPeakFilterGainSlider.setBounds (
        padding + widthAndHeight * 2,
        paddingTop + (labelHeight + widthAndHeight) * 2,
        widthAndHeight,
        widthAndHeight);

    //

    mPowerAmpHighShelfFrequencySlider.setBounds (
        padding + widthAndHeight * 0,
        paddingTop + (labelHeight + widthAndHeight) * 3,
        widthAndHeight,
        widthAndHeight);
    mPowerAmpHighShelfQualitySlider.setBounds (
        padding + widthAndHeight * 1,
        paddingTop + (labelHeight + widthAndHeight) * 3,
        widthAndHeight,
        widthAndHeight);
    mPowerAmpHighShelfGainSlider.setBounds (
        padding + widthAndHeight * 2,
        paddingTop + (labelHeight + widthAndHeight) * 3,
        widthAndHeight,
        widthAndHeight);

    //

    mReverbBlendSlider.setBounds (
        padding + widthAndHeight * 0,
        paddingTop + (labelHeight + widthAndHeight) * 4,
        widthAndHeight,
        widthAndHeight);

    mOutputGainSlider.setBounds (
        padding + widthAndHeight * 1,
        paddingTop + (labelHeight + widthAndHeight) * 4,
        widthAndHeight,
        widthAndHeight);
}
