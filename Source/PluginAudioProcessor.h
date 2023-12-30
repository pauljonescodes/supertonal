#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "PluginPresetManager.h"

class PluginAudioProcessor : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void reset() override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged (const juce::String& parameterID, float newValue) override;

private:
    std::unique_ptr<juce::AudioProcessorValueTreeState> mAudioProcessorValueTreeStatePtr;
    std::unique_ptr<juce::AudioFormatManager> mAudioFormatManagerPtr;
    std::unique_ptr<PluginPresetManager> mPresetManagerPtr;
    
    bool mOverdriveTanhWaveShaperOn = true;
    bool mOverdriveSoftClipWaveShaperOn = true;
    bool mAmpImpulseResponseConvolutionOn = true;
    bool mAmpLowShelfFilterOn = true;
    bool mAmpMidPeakFilterOn = true;
    bool mAmpHighShelfFilterOn = true;
    bool mCabImpulseResponseConvolutionOn = true;

    std::unique_ptr<juce::dsp::ConvolutionMessageQueue> mQueue;
    std::unique_ptr<juce::dsp::Gain<float>> mInputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mOverdriveTanhWaveShaperPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mOverdriveSoftClipWaveShaperPtr;
    std::unique_ptr<juce::dsp::Bias<float>> mOverdriveBiasPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mOverdriveGainPtr;
    std::unique_ptr<juce::dsp::Convolution> mAmpImpulseResponseConvolutionPtr;
    std::unique_ptr<juce::dsp::Compressor<float>> mAmpCompressorPtr;
    std::unique_ptr<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> mAmpLowShelfFilterPtr;
    std::unique_ptr<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> mAmpMidPeakFilterPtr;
    std::unique_ptr<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> mAmpHighShelfFilterPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mAmpGainPtr;
    std::unique_ptr<juce::dsp::Convolution> mCabinetImpulseResponseConvolutionPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mOutputGainPtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)
};
