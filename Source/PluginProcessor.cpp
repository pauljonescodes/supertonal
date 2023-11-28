#include "PluginProcessor.h"
#include "BinaryData.h"
#include "Constants.h"
#include "PluginEditor.h"

using namespace juce;
using namespace std;
using namespace constants;

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
#if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
                          .withInput ("Input", AudioChannelSet::stereo(), true)
    #endif
                          .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
            ),
      mProcessorValueTreeState (
          *this,
          nullptr,
          juce::Identifier (pvtsIdentifier),
          {
              make_unique<AudioParameterFloat> (ParameterID (inputGainParameterId, 1), inputGainParameterName, gainMinimumValue, gainMaximumValue, gainDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (overdriveBiasParameterId, 1), overdriveBiasParameterName, biasMinimumValue, biasMaximumValue, biasDefaultValue),
              make_unique<AudioParameterBool> (ParameterID (overdriveTanhWaveShaperBypassParameterId, 1), overdriveTanhWaveShaperBypassParameterName, false),
              make_unique<AudioParameterBool> (ParameterID (overdriveSoftClipWaveShaperBypassParameterId, 1), overdriveSoftClipWaveShaperBypassParameterName, false),
              make_unique<AudioParameterFloat> (ParameterID (overdriveGainParameterId, 1), overdriveGainParameterName, gainMinimumValue, gainMaximumValue, gainDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpGainParameterId, 1), powerAmpGainParameterName, gainMinimumValue, gainMaximumValue, gainDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpLowShelfFrequencyParameterId, 1), powerAmpLowShelfFrequencyParamName, frequencyMinimumValue, frequencyMaximumValue, lowShelfFrequencyDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpLowShelfQualityParameterId, 1), powerAmpLowShelfQualityParameterName, qualityMinimumValue, qualityMaximumValue, qualityDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpLowShelfGainParameterId, 1), powerAmpLowShelfGainParameterName, gainMinimumValue, gainMaximumValue, gainDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpPeakFilterFrequencyParameterId, 1), powerAmpPeakFilterFrequencyParameterName, frequencyMinimumValue, frequencyMaximumValue, peakFilterFrequencyDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpPeakFilterQualityParameterId, 1), powerAmpPeakFilterQualityParameterName, qualityMinimumValue, qualityMaximumValue, qualityDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpPeakFilterGainParameterId, 1), powerAmpPeakFilterGainParameterName, 0.01, gainMaximumValue, 0.01),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpHighShelfFrequencyParameterId, 1), powerAmpHighShelfFrequencyParameterName, frequencyMinimumValue, frequencyMaximumValue, highShelfFrequencyDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpHighShelfQualityParameterId, 1), powerAmpHighShelfQualityParameterName, qualityMinimumValue, qualityMaximumValue, qualityDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (powerAmpHighShelfGainParameterId, 1), powerAmpHighShelfGainParameterName, gainMinimumValue, gainMaximumValue, gainDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (reverbBlendParameterId, 1), reverbBlendParameterName, blendMinimumValue, blendMaximumValue, reverbBlendDefaultValue),
              make_unique<AudioParameterFloat> (ParameterID (outputGainParameterId, 1), outputGainParameterName, gainMinimumValue, gainMaximumValue, gainDefaultValue),
          })
{
    mFormatManager.registerBasicFormats();

    mInputGainParameter = mProcessorValueTreeState.getParameter (inputGainParameterId);
    mOverdriveBiasParameter = mProcessorValueTreeState.getParameter (overdriveBiasParameterId);
    mOverdriveTanhWaveShaperBypass = dynamic_cast<AudioParameterBool*> (mProcessorValueTreeState.getParameter (overdriveTanhWaveShaperBypassParameterId));
    mOverdriveSoftClipWaveShaperBypass = dynamic_cast<AudioParameterBool*> (mProcessorValueTreeState.getParameter (overdriveSoftClipWaveShaperBypassParameterId));
    mOverdriveGainParameter = mProcessorValueTreeState.getParameter (overdriveGainParameterId);
    mPowerAmpGainParameter = mProcessorValueTreeState.getParameter (powerAmpGainParameterId);
    mPowerAmpLowShelfFrequencyParameter = mProcessorValueTreeState.getParameter (powerAmpLowShelfFrequencyParameterId);
    mPowerAmpLowShelfQualityParameter = mProcessorValueTreeState.getParameter (powerAmpLowShelfQualityParameterId);
    mPowerAmpLowShelfGainParameter = mProcessorValueTreeState.getParameter (powerAmpLowShelfGainParameterId);
    mPowerAmpPeakFilterFrequencyParameter = mProcessorValueTreeState.getParameter (powerAmpPeakFilterFrequencyParameterId);
    mPowerAmpPeakFilterQualityParameter = mProcessorValueTreeState.getParameter (powerAmpPeakFilterQualityParameterId);
    mPowerAmpPeakFilterGainParameter = mProcessorValueTreeState.getParameter (powerAmpPeakFilterGainParameterId);
    mPowerAmpHighShelfFrequencyParameter = mProcessorValueTreeState.getParameter (powerAmpHighShelfFrequencyParameterId);
    mPowerAmpHighShelfQualityParameter = mProcessorValueTreeState.getParameter (powerAmpHighShelfQualityParameterId);
    mPowerAmpHighShelfGainParameter = mProcessorValueTreeState.getParameter (powerAmpHighShelfGainParameterId);
    mReverbBlendParameter = mProcessorValueTreeState.getParameter (reverbBlendParameterId);
    mOutputGainParameter = mProcessorValueTreeState.getParameter (outputGainParameterId);

    mProcessorValueTreeState.addParameterListener (inputGainParameterId, this);
    mProcessorValueTreeState.addParameterListener (overdriveBiasParameterId, this);
    mProcessorValueTreeState.addParameterListener (overdriveTanhWaveShaperBypassParameterId, this);
    mProcessorValueTreeState.addParameterListener (overdriveGainParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpGainParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpLowShelfFrequencyParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpLowShelfQualityParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpLowShelfGainParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpPeakFilterFrequencyParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpPeakFilterQualityParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpPeakFilterGainParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpHighShelfFrequencyParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpHighShelfQualityParameterId, this);
    mProcessorValueTreeState.addParameterListener (powerAmpHighShelfGainParameterId, this);
    mProcessorValueTreeState.addParameterListener (reverbBlendParameterId, this);
    mProcessorValueTreeState.addParameterListener (outputGainParameterId, this);

    setSmoothedTargetValues();
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    mInputGain.prepare (spec);

    mOverdriveBias.prepare (spec);

    mOverdriveTanhWaveShaper.prepare (spec);
    mOverdriveTanhWaveShaper.functionToUse = [] (float x) {
        return std::tanh (x);
    };

    mOverdriveSoftClipWaveShaper.prepare (spec);
    mOverdriveSoftClipWaveShaper.functionToUse = [] (float x) {
        return x / (std::abs (x) + 1.0f);
    };

    mOverdriveGain.prepare (spec);

    mPowerAmpImpulseResponseConvolution.prepare (spec);
    mPowerAmpImpulseResponseConvolution.loadImpulseResponse (
        BinaryData::VTM60_low_pregain70_low50_mid50_high50_presence50_wav,
        BinaryData::VTM60_low_pregain70_low50_mid50_high50_presence50_wavSize,
        dsp::Convolution::Stereo::yes,
        dsp::Convolution::Trim::no,
        BinaryData::VTM60_low_pregain70_low50_mid50_high50_presence50_wavSize,
        dsp::Convolution::Normalise::yes);

    mPowerAmpGain.prepare (spec);
    mPowerAmpLowShelfFilter.prepare (spec);
    mPowerAmpPeakFilter.prepare (spec);
    mPowerAmpHighShelfFilter.prepare (spec);

    mCabinetImpulseResponseConvolution.prepare (spec);
    mCabinetImpulseResponseConvolution.loadImpulseResponse (
        BinaryData::cory_bread_and_butter_normalized_wav,
        BinaryData::cory_bread_and_butter_normalized_wavSize,
        dsp::Convolution::Stereo::yes,
        dsp::Convolution::Trim::no,
        BinaryData::cory_bread_and_butter_normalized_wavSize,
        dsp::Convolution::Normalise::yes);

    mReverbImpulseResponseConvolution.prepare (spec);
    mReverbImpulseResponseConvolution.loadImpulseResponse (
        BinaryData::Guitar_Plate_aif,
        BinaryData::Guitar_Plate_aifSize,
        dsp::Convolution::Stereo::yes,
        dsp::Convolution::Trim::no,
        BinaryData::Guitar_Plate_aifSize,
        dsp::Convolution::Normalise::yes);

    reset();
}

void PluginProcessor::reset()
{
    mInputGain.reset();
    mOverdriveBias.reset();
    mOverdriveTanhWaveShaper.reset();
    mOverdriveSoftClipWaveShaper.reset();
    mOverdriveGain.reset();
    mPowerAmpImpulseResponseConvolution.reset();
    mPowerAmpGain.reset();
    mPowerAmpLowShelfFilter.reset();
    mPowerAmpPeakFilter.reset();
    mPowerAmpHighShelfFilter.reset();
    mCabinetImpulseResponseConvolution.reset();
    mReverbImpulseResponseConvolution.reset();
    mOutputGain.reset();
}

void PluginProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    getSmoothedNextValues();

    auto audioBlock = dsp::AudioBlock<float> (buffer);
    auto context = dsp::ProcessContextReplacing<float> (audioBlock);

    mInputGain.process (context);
    mOverdriveBias.process (context);
    mOverdriveGain.process (context);

    if (!mOverdriveTanhWaveShaperBypass->get())
    {
        mOverdriveTanhWaveShaper.process (context);
    }

    if (!mOverdriveSoftClipWaveShaperBypass->get())
    {
        mOverdriveSoftClipWaveShaper.process (context);
    }

    mPowerAmpImpulseResponseConvolution.process (context);

    mPowerAmpLowShelfFilter.process (context);
    mPowerAmpPeakFilter.process (context);
    mPowerAmpHighShelfFilter.process (context);
    mPowerAmpGain.process (context);

    mCabinetImpulseResponseConvolution.process (context);

 // Capture the dry signal just before the reverb
    AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf (buffer);

    // Apply reverb (wet signal)
    mReverbImpulseResponseConvolution.process (context);

    // Blend wet and dry signals
    float reverbBlend = mReverbBlendSmoothedValue.getCurrentValue();
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* drySignal = dryBuffer.getReadPointer (channel);
        auto* wetSignal = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            wetSignal[sample] = drySignal[sample] * (1.0f - reverbBlend) + wetSignal[sample] * reverbBlend;
        }
    }

    mOutputGain.process (context);
}

void PluginProcessor::getSmoothedNextValues()
{
    auto sampleRate = getSampleRate();

    mInputGain.setGainDecibels (mInputGainSmoothedValue.getNextValue());
    mOverdriveBias.setBias (mOverdriveBiasSmoothedValue.getNextValue());
    mOverdriveGain.setGainDecibels (mOverdriveGainSmoothedValue.getNextValue());

    mPowerAmpGain.setGainDecibels (mPowerAmpGainSmoothedValue.getNextValue());

    *mPowerAmpLowShelfFilter.state = *dsp::IIR::Coefficients<float>::makeLowShelf (
        sampleRate,
        jmax (mPowerAmpLowShelfFrequencySmoothedValue.getNextValue(), frequencyMinimumValue),
        jmax (mPowerAmpLowShelfQualitySmoothedValue.getNextValue(), qualityMinimumValue),
        Decibels::decibelsToGain (mPowerAmpLowShelfGainSmoothedValue.getNextValue()));

    *mPowerAmpPeakFilter.state = *dsp::IIR::Coefficients<float>::makePeakFilter (
        sampleRate,
        jmax (mPowerAmpPeakFilterFrequencySmoothedValue.getNextValue(), frequencyMinimumValue),
        jmax (mPowerAmpPeakFilterQualitySmoothedValue.getNextValue(), qualityMinimumValue),
        Decibels::decibelsToGain (mPowerAmpPeakFilterGainSmoothedValue.getNextValue()));

    *mPowerAmpHighShelfFilter.state = *dsp::IIR::Coefficients<float>::makeHighShelf (
        sampleRate,
        jmax (mPowerAmpHighShelfFrequencySmoothedValue.getNextValue(), frequencyMinimumValue),
        jmax (mPowerAmpHighShelfQualitySmoothedValue.getNextValue(), qualityMinimumValue),
        Decibels::decibelsToGain (mPowerAmpHighShelfGainSmoothedValue.getNextValue()));

    mOutputGain.setGainDecibels (mOutputGainSmoothedValue.getNextValue());
}

void PluginProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    setSmoothedTargetValues();
}

void PluginProcessor::setSmoothedTargetValues()
{
    mInputGainSmoothedValue.setTargetValue (mInputGainParameter->convertFrom0to1 (mInputGainParameter->getValue()));
    mOverdriveBiasSmoothedValue.setTargetValue (mOverdriveBiasParameter->convertFrom0to1 (mOverdriveBiasParameter->getValue()));
    mOverdriveGainSmoothedValue.setTargetValue (mOverdriveGainParameter->convertFrom0to1 (mOverdriveGainParameter->getValue()));

    mPowerAmpGainSmoothedValue.setTargetValue (mPowerAmpGainParameter->convertFrom0to1 (mPowerAmpGainParameter->getValue()));
    mPowerAmpLowShelfFrequencySmoothedValue.setTargetValue (mPowerAmpLowShelfFrequencyParameter->convertFrom0to1 (mPowerAmpLowShelfFrequencyParameter->getValue()));
    mPowerAmpLowShelfQualitySmoothedValue.setTargetValue (mPowerAmpLowShelfQualityParameter->convertFrom0to1 (mPowerAmpLowShelfQualityParameter->getValue()));
    mPowerAmpLowShelfGainSmoothedValue.setTargetValue (mPowerAmpLowShelfGainParameter->convertFrom0to1 (mPowerAmpLowShelfGainParameter->getValue()));

    mPowerAmpPeakFilterFrequencySmoothedValue.setTargetValue (mPowerAmpPeakFilterFrequencyParameter->convertFrom0to1 (mPowerAmpPeakFilterFrequencyParameter->getValue()));
    mPowerAmpPeakFilterQualitySmoothedValue.setTargetValue (mPowerAmpPeakFilterQualityParameter->convertFrom0to1 (mPowerAmpPeakFilterQualityParameter->getValue()));
    mPowerAmpPeakFilterGainSmoothedValue.setTargetValue (mPowerAmpPeakFilterGainParameter->convertFrom0to1 (mPowerAmpPeakFilterGainParameter->getValue()));

    mPowerAmpHighShelfFrequencySmoothedValue.setTargetValue (mPowerAmpHighShelfFrequencyParameter->convertFrom0to1 (mPowerAmpHighShelfFrequencyParameter->getValue()));
    mPowerAmpHighShelfQualitySmoothedValue.setTargetValue (mPowerAmpHighShelfQualityParameter->convertFrom0to1 (mPowerAmpHighShelfQualityParameter->getValue()));
    mPowerAmpHighShelfGainSmoothedValue.setTargetValue (mPowerAmpHighShelfGainParameter->convertFrom0to1 (mPowerAmpHighShelfGainParameter->getValue()));

    mReverbBlendSmoothedValue.setTargetValue (mReverbBlendParameter->convertFrom0to1 (mReverbBlendParameter->getValue()));

    mOutputGainSmoothedValue.setTargetValue (mOutputGainParameter->convertFrom0to1 (mOutputGainParameter->getValue()));
}

void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = mProcessorValueTreeState.copyState();
    unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (mProcessorValueTreeState.state.getType()))
        {
            mProcessorValueTreeState.replaceState (ValueTree::fromXml (*xmlState));
        }
    }
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    ignoreUnused (index);
}

const String PluginProcessor::getProgramName (int index)
{
    ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const String& newName)
{
    ignoreUnused (index, newName);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
    #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
#endif
}

bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}
