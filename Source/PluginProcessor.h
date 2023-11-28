#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#if (MSVC)
    #include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

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

    juce::AudioProcessorValueTreeState mProcessorValueTreeState;
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void getSmoothedNextValues();
    void setSmoothedTargetValues();

private:
    // Begin custom

    juce::dsp::ConvolutionMessageQueue mQueue;
    juce::AudioFormatManager mFormatManager;

    juce::RangedAudioParameter* mInputGainParameter = nullptr;
    juce::SmoothedValue<float> mInputGainSmoothedValue;
    juce::dsp::Gain<float> mInputGain;

    juce::RangedAudioParameter* mOverdriveBiasParameter = nullptr;
    juce::SmoothedValue<float> mOverdriveBiasSmoothedValue;
    juce::dsp::Bias<float> mOverdriveBias;

    juce::AudioParameterBool* mOverdriveTanhWaveShaperBypass = nullptr;
    juce::dsp::WaveShaper<float> mOverdriveTanhWaveShaper;

    juce::AudioParameterBool* mOverdriveSoftClipWaveShaperBypass = nullptr;
    juce::dsp::WaveShaper<float> mOverdriveSoftClipWaveShaper;

    juce::RangedAudioParameter* mOverdriveGainParameter = nullptr;
    juce::SmoothedValue<float> mOverdriveGainSmoothedValue;
    juce::dsp::Gain<float> mOverdriveGain;

    juce::dsp::Convolution mPowerAmpImpulseResponseConvolution { juce::dsp::Convolution::NonUniform { 2048 }, mQueue };

    juce::RangedAudioParameter* mPowerAmpGainParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpGainSmoothedValue;
    juce::dsp::Gain<float> mPowerAmpGain;

    juce::RangedAudioParameter* mPowerAmpLowShelfFrequencyParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpLowShelfFrequencySmoothedValue;

    juce::RangedAudioParameter* mPowerAmpLowShelfQualityParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpLowShelfQualitySmoothedValue;

    juce::RangedAudioParameter* mPowerAmpLowShelfGainParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpLowShelfGainSmoothedValue;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mPowerAmpLowShelfFilter;

    juce::RangedAudioParameter* mPowerAmpPeakFilterFrequencyParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpPeakFilterFrequencySmoothedValue;

    juce::RangedAudioParameter* mPowerAmpPeakFilterQualityParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpPeakFilterQualitySmoothedValue;

    juce::RangedAudioParameter* mPowerAmpPeakFilterGainParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpPeakFilterGainSmoothedValue;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mPowerAmpPeakFilter;

    juce::RangedAudioParameter* mPowerAmpHighShelfFrequencyParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpHighShelfFrequencySmoothedValue;

    juce::RangedAudioParameter* mPowerAmpHighShelfQualityParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpHighShelfQualitySmoothedValue;

    juce::RangedAudioParameter* mPowerAmpHighShelfGainParameter = nullptr;
    juce::SmoothedValue<float> mPowerAmpHighShelfGainSmoothedValue;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mPowerAmpHighShelfFilter;

    juce::dsp::Convolution mCabinetImpulseResponseConvolution { juce::dsp::Convolution::NonUniform { 2048 }, mQueue };
    
    juce::RangedAudioParameter* mReverbBlendParameter = nullptr;
    juce::SmoothedValue<float> mReverbBlendSmoothedValue;
    juce::dsp::Convolution mReverbImpulseResponseConvolution { juce::dsp::Convolution::NonUniform { 2048 }, mQueue };

    juce::RangedAudioParameter* mOutputGainParameter = nullptr;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> mOutputGainSmoothedValue;
    juce::dsp::Gain<float> mOutputGain;

    // End custom

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
