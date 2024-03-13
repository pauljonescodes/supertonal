#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <JuceHeader.h>

#include "PluginPresetManager.h"
#include "Processors/Saturators/MouseDrive.h"
#include "Processors/Saturators/TubeScreamer.h"
#include "Processors/Equilisers/GraphicEqualiser.h"
#include "Processors/Equilisers/AmplifierEqualiser.h"
#include "Processors/Equilisers/InstrumentEqualiser.h"
#include "Processors/CTAGDRC/dsp/include/Compressor.h"
#include "Processors/Other/Bitcrusher.h"
#include "Processors/Modulators/Phaser.h"
#include "Processors/Modulators/Chorus.h"
#include "Processors/Modulators/Flanger.h"

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

    foleys::LevelMeterSource& getInputMeterSource()
    {
        return *mInputLevelMeterSourcePtr;
    }

    foleys::LevelMeterSource& getOutputMeterSource()
    {
        return *mOutputLevelMeterSourcePtr;
    }

    juce::AudioProcessorValueTreeState& getAudioProcessorValueTreeState()
    {
        return *mAudioProcessorValueTreeStatePtr;
    }

    PluginPresetManager& getPresetManager()
    {
        return *mPresetManagerPtr;
    }

    juce::UndoManager& getUndoManager()
    {
        return *mUndoManager;
    }

private:
    std::unique_ptr <juce::UndoManager> mUndoManager;
    std::unique_ptr<juce::AudioProcessorValueTreeState> mAudioProcessorValueTreeStatePtr;
    std::unique_ptr<PluginPresetManager> mPresetManagerPtr;
    std::unique_ptr<juce::AudioFormatManager> mAudioFormatManagerPtr;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> mBpmSmoothedValue;

    std::unique_ptr <foleys::LevelMeterSource> mInputLevelMeterSourcePtr;
    std::unique_ptr <foleys::LevelMeterSource> mOutputLevelMeterSourcePtr;

    std::unique_ptr<juce::dsp::Gain<float>> mInputGainPtr;

    std::unique_ptr<juce::dsp::NoiseGate<float>> mNoiseGate;

    bool mIsPreCompressorOn = false;
    std::unique_ptr<juce::dsp::Compressor<float>> mPreCompressorPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mPreCompressorGainPtr;
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> mPreCompressorGainSmoothedValue;
    bool mIsPreCompressorAutoMakeup = false;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mPreCompressorDryWetMixerPtr;

    bool mIsTubeScreamerOn = false;
    std::unique_ptr<TubeScreamer> mTubeScreamerPtr;

    bool mIsMouseDriveOn = false;
    std::unique_ptr<MouseDrive> mMouseDrivePtr;

    bool mIsGraphicEqualiserOn = false;
    std::unique_ptr<GraphicEqualiser> mGraphicEqualiser;

    bool mIsStage1On = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage1Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage1InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage1WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage1OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage1DryWetMixerPtr;

    bool mIsStage2On = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage2Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage2InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage2WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage2OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage2DryWetMixerPtr;

    bool mIsStage3On = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage3Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage3InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage3WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage3OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage3DryWetMixerPtr;

    bool mIsStage4On = false;
    std::unique_ptr<juce::AudioBuffer<float>> mStage4Buffer;
    std::unique_ptr<juce::dsp::Gain<float>> mStage4InputGainPtr;
    std::unique_ptr<juce::dsp::WaveShaper<float>> mStage4WaveShaperPtr;
    std::unique_ptr<juce::dsp::Gain<float>> mStage4OutputGainPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mStage4DryWetMixerPtr;

    std::unique_ptr<juce::dsp::Bias<float>> mBiasPtr;
    std::unique_ptr<AmplifierEqualiser> mAmplifierEqualiser;
    
    bool mIsDelayOn = false;
    bool mDelayBpmSynced = false;
    bool mDelayIsLinked = true;
    float mDelayLeftMilliseconds = 30;
    float mDelayRightMilliseconds = 30;
    float mDelayLeftPerBeatDivision = 2.0f;
    float mDelayRightPerBeatDivision = 2.0f;
    float mDelayFeedback = 0.5f;
    std::unique_ptr<juce::dsp::DelayLine<float>> mDelayLineLeftPtr;
    std::unique_ptr<juce::dsp::DelayLine<float>> mDelayLineRightPtr;
    std::unique_ptr<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> mDelayLowPassFilterPtr;
    std::unique_ptr<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> mDelayHighPassFilterPtr;
    std::unique_ptr<juce::dsp::DryWetMixer<float>> mDelayLineDryWetMixerPtr;

    std::unique_ptr<Chorus> mChorusPtr;
    std::unique_ptr<Phaser> mPhaserPtr;
    std::unique_ptr<Flanger> mFlangerPtr;
    std::unique_ptr<Bitcrusher> mBitcrusherPtr;

    bool mIsCabImpulseResponseConvolutionOn = true;
    int mCabinetImpulseResponseIndex = 0;
    std::unique_ptr<juce::dsp::ConvolutionMessageQueue> mConvolutionMessageQueuePtr;
    std::unique_ptr<juce::dsp::Convolution> mCabinetImpulseResponseConvolutionPtr;

    bool mIsLofi = false;
    std::unique_ptr<juce::dsp::Convolution> mLofiImpulseResponseConvolutionPtr;

    bool mIsReverbOn = false;
    std::unique_ptr<juce::dsp::Reverb> mReverbPtr;
    
    std::unique_ptr<juce::dsp::Gain<float>> mCabinetGainPtr;

    std::unique_ptr<InstrumentEqualiser> mInstrumentEqualiserPtr;

    bool mIsInstrumentCompressorPreEqualiser;
    std::unique_ptr<Compressor> mInstrumentCompressorPtr;
    //LevelEnvelopeFollower inLevelFollower;
    //LevelEnvelopeFollower outLevelFollower;

    bool mIsLimiterOn = true;
    std::unique_ptr<juce::dsp::Limiter<float>> mLimiterPtr;

    std::unique_ptr<juce::dsp::Gain<float>> mOutputGainPtr;

    bool mIsBypassOn = false;

    void loadImpulseResponseFromState();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)
};
