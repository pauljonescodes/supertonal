#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "PluginPresetManager.h"
#include "Processors/MouseDrive.h"
#include "Processors/TubeScreamer.h"
#include "Processors/GraphicEqualiser.h"
#include "Processors/AmplifierEqualiser.h"
#include "Processors/InstrumentEqualiser.h"

class PluginAudioProcessor : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener, juce::ValueTree::Listener
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
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

private:
    std::unique_ptr<juce::AudioProcessorValueTreeState> mAudioProcessorValueTreeStatePtr;
    std::unique_ptr<PluginPresetManager> mPresetManagerPtr;
    std::unique_ptr<juce::AudioFormatManager> mAudioFormatManagerPtr;

    std::unique_ptr<juce::dsp::Gain<float>> mInputGainPtr;

    std::unique_ptr<juce::dsp::NoiseGate<float>> mNoiseGate;

    bool mPreCompressorIsOn = false;
    std::unique_ptr<juce::dsp::Compressor<float>> mPreCompressorPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mPreCompressorGainPtr;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> mPreCompressorGainSmoothedValue;
    bool mPreCompressorAutoMakeup = false;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mPreCompressorDryWetMixerPtr;

    bool mTubeScreamerIsOn = false;
    std::unique_ptr<TubeScreamer> mTubeScreamerPtr;

    bool mMouseDriveIsOn = false;
    std::unique_ptr<MouseDrive> mMouseDrivePtr;

    bool mGraphicEqualiserIsOn = false;
    std::unique_ptr<GraphicEqualiser> mGraphicEqualiser;

    bool mStagesAreParallel = false;

    bool mStage1IsOn = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage1Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage1InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage1WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage1OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage1DryWetMixerPtr;

    bool mStage2IsOn = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage2Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage2InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage2WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage2OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage2DryWetMixerPtr;

    bool mStage3IsOn = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage3Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage3InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage3WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage3OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage3DryWetMixerPtr;

    bool mStage4IsOn = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage4Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage4InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage4WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage4OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage4DryWetMixerPtr;

    std::unique_ptr<juce::dsp::Bias<float>> mBiasPtr;
    std::unique_ptr<AmplifierEqualiser> mAmplifierEqualiser;
    
    std::unique_ptr<juce::dsp::Compressor<float>> mPostCompressorPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mCompressorGainPtr;
    
    bool mDelayOn = false;
    bool mDelayBpmSynced = false;
    float mDelayLeftMilliseconds = 30;
    float mDelayRightMilliseconds = 30;
    float mDelayFeedback = 0.5f;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> mBpmSmoothedValue;
    std::unique_ptr<juce::dsp::DelayLine<float>> mDelayLineLeftPtr;
    std::unique_ptr<juce::dsp::DelayLine<float>> mDelayLineRightPtr;

    std::unique_ptr<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> mDelayLowPassFilterPtr;
    std::unique_ptr<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> mDelayHighPassFilterPtr;
    
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mDelayLineDryWetMixerPtr;

    bool mChorusOn = false;
    std::unique_ptr<juce::dsp::Chorus<float>> mChorusPtr;

    bool mPhaserOn = false;
    std::unique_ptr<juce::dsp::Phaser<float>> mPhaserPtr;

    bool mCabImpulseResponseConvolutionIsOn = true;
    std::unique_ptr<juce::dsp::ConvolutionMessageQueue> mConvolutionMessageQueuePtr;
    std::unique_ptr<juce::dsp::Convolution> mCabinetImpulseResponseConvolutionPtr;

    bool mReverbOn = false;
    std::unique_ptr<juce::dsp::Reverb> mReverb;
    
    std::unique_ptr<juce::dsp::Gain<float>> mCabinetGainPtr;

    std::unique_ptr<InstrumentEqualiser> mInstrumentEqualiser;

    bool mLimiterOn = true;
    std::unique_ptr<juce::dsp::Limiter<float>> mLimiter;

    std::unique_ptr<juce::dsp::Gain<float>> mOutputGainPtr;

    bool mBypassIsOn = false;

    void loadImpulseResponseFromState();
    void checkForInvalidSamples (const juce::dsp::AudioBlock<float>& blockToCheck);
    float calculateRMS(juce::AudioBuffer<float>& buffer, int numChannels, int numSamples);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)
};
