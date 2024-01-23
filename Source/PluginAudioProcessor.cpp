#include "PluginAudioProcessor.h"
#include "BinaryData.h"
#include "PluginAudioProcessorEditor.h"
#include "PluginAudioParameters.h"
#include <cassert>
#include "PluginUtils.h"

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
	: AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	),
	mAudioProcessorValueTreeStatePtr(std::make_unique<juce::AudioProcessorValueTreeState>(
		*this,
		nullptr,
		juce::Identifier(apvts::identifier),
		createParameterLayout())),
	mPresetManagerPtr(std::make_unique<PluginPresetManager>(*mAudioProcessorValueTreeStatePtr.get())),
	mAudioFormatManagerPtr(std::make_unique<juce::AudioFormatManager>()),

	mNoiseGate(std::make_unique<juce::dsp::NoiseGate<float>>()),
	mPreCompressorPtr(std::make_unique<juce::dsp::Compressor<float>>()),

	mTubeScreamerPtr(std::make_unique<TubeScreamer>()),
	mMouseDrivePtr(std::make_unique<MouseDrive>()),

	mStage1Buffer(std::make_unique<juce::AudioBuffer<float>>()),
	mStage1InputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage1WaveShaperPtr(std::make_unique<juce::dsp::WaveShaper<float>>()),
	mStage1OutputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage1DryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),

	mStage2Buffer(std::make_unique<juce::AudioBuffer<float>>()),
	mStage2InputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage2WaveShaperPtr(std::make_unique<juce::dsp::WaveShaper<float>>()),
	mStage2OutputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage2DryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),

	mStage3Buffer(std::make_unique<juce::AudioBuffer<float>>()),
	mStage3InputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage3WaveShaperPtr(std::make_unique<juce::dsp::WaveShaper<float>>()),
	mStage3OutputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage3DryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),

	mStage4Buffer(std::make_unique<juce::AudioBuffer<float>>()),
	mStage4InputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage4WaveShaperPtr(std::make_unique<juce::dsp::WaveShaper<float>>()),
	mStage4OutputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mStage4DryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),

	mBiasPtr(std::make_unique<juce::dsp::Bias<float>>()),
	mPostCompressorPtr(std::make_unique<juce::dsp::Compressor<float>>()),
	mCompressorGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	
	mHighPassFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mMidPeakFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mHighShelfFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mLowPassFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),

	mDelayLinePtr(std::make_unique<juce::dsp::DelayLine<float>>()),
	mDelayLineDryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),

	mChorusPtr(std::make_unique<juce::dsp::Chorus<float>>()),
	mPhaserPtr(std::make_unique<juce::dsp::Phaser<float>>()),

	mConvolutionMessageQueuePtr(std::make_unique<juce::dsp::ConvolutionMessageQueue>()),
	mCabinetImpulseResponseConvolutionPtr(std::make_unique<juce::dsp::Convolution>(juce::dsp::Convolution::NonUniform{ 128 }, * mConvolutionMessageQueuePtr.get())),
	
	mReverb(std::make_unique<juce::dsp::Reverb>()),

	mCabinetGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mLimiter(std::make_unique<juce::dsp::Limiter<float>>()),

	mOutputGainPtr(std::make_unique<juce::dsp::Gain<float>>())
{
	mAudioFormatManagerPtr->registerBasicFormats();

	mAudioProcessorValueTreeStatePtr->state.addListener(this);
	for (const auto& parameterIdAndEnum : apvts::parameterIdToEnumMap) {
		mAudioProcessorValueTreeStatePtr->addParameterListener(parameterIdAndEnum.first, this);
	}
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	juce::StringArray waveShaperIdsJuceStringArray;
	for (const auto& idToFunction : apvts::waveShaperIdToFunctionMap) {
		waveShaperIdsJuceStringArray.add(idToFunction.first);
	}

	juce::StringArray modeIdsJuceStringArray;
	for (const auto& item : apvts::stageModeIds) {
		modeIdsJuceStringArray.add(item);
	}

	for (const auto& parameterIdAndEnum : apvts::parameterIdToEnumMap)
	{
		auto& parameterId = parameterIdAndEnum.first;

		std::vector<std::string> parts;
		std::stringstream ss(parameterId);
		std::string item;

		while (std::getline(ss, item, '_')) {
			parts.push_back(item);
		}

		switch (parameterIdAndEnum.second)
		{
		case apvts::ParameterEnum::STAGE1_ON:
		case apvts::ParameterEnum::LIMITER_ON:
			layout.add(std::make_unique<juce::AudioParameterBool>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				true
				));
			break;
		case apvts::ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON:
		case apvts::ParameterEnum::BYPASS_ON:
		case apvts::ParameterEnum::TUBE_SCREAMER_ON:
		case apvts::ParameterEnum::MOUSE_DRIVE_ON:
		case apvts::ParameterEnum::STAGE2_ON:
		case apvts::ParameterEnum::STAGE3_ON:
		case apvts::ParameterEnum::STAGE4_ON:
		case apvts::ParameterEnum::REVERB_ON:
			layout.add(std::make_unique<juce::AudioParameterBool>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				false
				));
			break;
		case apvts::ParameterEnum::CHORUS_DEPTH:
		case apvts::ParameterEnum::PHASER_DEPTH:
		case apvts::ParameterEnum::CHORUS_FEEDBACK:
		case apvts::ParameterEnum::PHASER_FEEDBACK:
		case apvts::ParameterEnum::DELAY_FEEDBACK:
		case apvts::ParameterEnum::STAGE1_DRY_WET:
		case apvts::ParameterEnum::STAGE2_DRY_WET:
		case apvts::ParameterEnum::STAGE3_DRY_WET:
		case apvts::ParameterEnum::STAGE4_DRY_WET:
		case apvts::ParameterEnum::CHORUS_MIX:
		case apvts::ParameterEnum::DELAY_DRY_WET:
		case apvts::ParameterEnum::PHASER_MIX:
		case apvts::ParameterEnum::REVERB_MIX:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::zeroToOneLinearNormalisableRange,
				apvts::defaultValueOff
				));
			break;
		case apvts::ParameterEnum::STAGE1_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE1_OUTPUT_GAIN:
		case apvts::ParameterEnum::STAGE2_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE2_OUTPUT_GAIN:
		case apvts::ParameterEnum::STAGE3_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE3_OUTPUT_GAIN:
		case apvts::ParameterEnum::STAGE4_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE4_OUTPUT_GAIN:
		case apvts::ParameterEnum::POST_COMPRESSOR_GAIN:
		case apvts::ParameterEnum::CABINET_OUTPUT_GAIN:
		case apvts::ParameterEnum::OUTPUT_GAIN:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNormalisableRange,
				apvts::gainDeciblesDefaultValue
				));
			break;
		case apvts::ParameterEnum::REVERB_SIZE:
		case apvts::ParameterEnum::REVERB_DAMPING:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::zeroToOneLogarithmicNormalisableRange,
				apvts::defaultValueOff
				));
			break;
		case apvts::ParameterEnum::REVERB_WIDTH:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::zeroToOneLinearNormalisableRange,
				apvts::defaultValueOn
				));
			break;
		case apvts::ParameterEnum::NOISE_GATE_THRESHOLD:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNegativeNormalisableRange,
				apvts::gainDecibelsMinimumValue
				));
			break;
		case apvts::ParameterEnum::PRE_COMPRESSOR_THRESHOLD:
		case apvts::ParameterEnum::POST_COMPRESSOR_THRESHOLD:
		case apvts::ParameterEnum::MOUSE_DRIVE_VOLUME:
		case apvts::ParameterEnum::TUBE_SCREAMER_LEVEL:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNegativeNormalisableRange,
				apvts::gainDeciblesDefaultValue
				));
			break;
		case apvts::ParameterEnum::LIMITER_THRESHOLD:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNegativeNormalisableRange,
				apvts::limiterThresholdDefaultValue
				));
			break;
		case apvts::ParameterEnum::STAGE1_WAVE_SHAPER:
		case apvts::ParameterEnum::STAGE2_WAVE_SHAPER:
		case apvts::ParameterEnum::STAGE3_WAVE_SHAPER:
		case apvts::ParameterEnum::STAGE4_WAVE_SHAPER:
			layout.add(std::make_unique<juce::AudioParameterChoice>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				waveShaperIdsJuceStringArray,
				1));
			break;
		case apvts::ParameterEnum::STAGE_MODE:
			layout.add(std::make_unique<juce::AudioParameterChoice>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				modeIdsJuceStringArray,
				0));
			break;
		case apvts::ParameterEnum::BIAS:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::negativeOneToOneLinearNormalisableRange,
				apvts::defaultValueOff
				));
			break;
		case apvts::ParameterEnum::PRE_COMPRESSOR_ATTACK:
		case apvts::ParameterEnum::POST_COMPRESSOR_ATTACK:
		case apvts::ParameterEnum::NOISE_GATE_ATTACK:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::attackNormalisableRange,
				apvts::attackMsDefaultValue
				));
			break;
		case apvts::ParameterEnum::PRE_COMPRESSOR_RELEASE:
		case apvts::ParameterEnum::POST_COMPRESSOR_RELEASE:
		case apvts::ParameterEnum::NOISE_GATE_RELEASE:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::releaseMsNormalisableRange,
				apvts::releaseMsDefaultValue
				));
			break;
		case apvts::ParameterEnum::PRE_COMPRESSOR_RATIO:
		case apvts::ParameterEnum::POST_COMPRESSOR_RATIO:
		case apvts::ParameterEnum::NOISE_GATE_RATIO:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::ratioNormalizableRange,
				apvts::ratioDefaultValue
				));
			break;
		case apvts::ParameterEnum::HIGH_PASS_FREQUENCY:
		case apvts::ParameterEnum::MID_PEAK_FREQUENCY:
		case apvts::ParameterEnum::HIGH_SHELF_FREQUENCY:
		case apvts::ParameterEnum::LOW_PASS_FREQUENCY:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::frequencyNormalisableRange,
				apvts::equalizationTypeIdToDefaultFrequencyMap.at(parts[0])
				));
			break;
		case apvts::ParameterEnum::HIGH_PASS_Q:
		case apvts::ParameterEnum::MID_PEAK_Q:
		case apvts::ParameterEnum::HIGH_SHELF_Q:
		case apvts::ParameterEnum::LOW_PASS_Q:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::qualityNormalisableRange,
				apvts::qualityDefaultValue
				));
			break;
		case apvts::ParameterEnum::MOUSE_DRIVE_DISTORTION:
		case apvts::ParameterEnum::TUBE_SCREAMER_DRIVE:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::zeroToOneLinearNormalisableRange,
				apvts::defaultValueOff
				));
			break;
		case apvts::ParameterEnum::CHORUS_FRACTION_OF_BEAT:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::fractionalTimeNormalizableRange,
				apvts::fractionalTimeDefaultValue
				));
			break;
		case apvts::ParameterEnum::PHASER_RATE_FRACTION_OF_BEAT:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::fractionalTimeNormalizableRange,
				apvts::fractionalTimeDefaultValue
				));
			break;
		case apvts::ParameterEnum::MID_PEAK_GAIN:
		case apvts::ParameterEnum::HIGH_SHELF_GAIN:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainLinearNormalisableRange,
				apvts::gainLinearDefaultValue
				));
			break;
		case apvts::ParameterEnum::LIMITER_RELEASE:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::releaseMsNormalisableRange,
				apvts::limiterReleaseDefaultValue
				));
			break;
		case apvts::ParameterEnum::CHORUS_CENTER_DELAY_FRACTION_OF_BEAT:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::fractionalTimeNormalizableRange,
				apvts::fractionalTimeDefaultValue
				));
			break;
		case apvts::ParameterEnum::PHASER_CENTER_FREQUENCY:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::frequencyNormalisableRange,
				apvts::phaserCenterFrequencyDefaultValue
				));
			break;
		case apvts::ParameterEnum::DELAY_TIME_FRACTIONAL_DENOMINATOR:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::fractionalTimeNormalizableRange,
				apvts::fractionalTimeDefaultValue
				));
			break;
		case apvts::ParameterEnum::TUBE_SCREAMER_DIODE_TYPE:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::tubeScreamerDiodeTypeNormalizableRange,
				apvts::tubeScreamerDiodeTypeDefaultValue
				));
			break;
		case apvts::ParameterEnum::TUBE_SCREAMER_DIODE_COUNT:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::tubeScreamerDiodeCountNormalizableRange,
				apvts::tubeScreamerDiodeCountDefaultValue
				));
			break;
		default:
			assert(false);
			break;
		}
	}

	return layout;
}

void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	juce::dsp::ProcessSpec spec;

	const auto numChannels = getTotalNumOutputChannels();

	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = numChannels;

	mNoiseGate->prepare(spec);
	mPreCompressorPtr->prepare(spec);

	mTubeScreamerPtr->prepare(spec);
	mMouseDrivePtr->prepare(spec);

	mStage1Buffer->setSize(numChannels, samplesPerBlock);
	mStage1InputGainPtr->prepare(spec);
	mStage1WaveShaperPtr->prepare(spec);
	mStage1OutputGainPtr->prepare(spec);
	mStage1DryWetMixerPtr->prepare(spec);

	mStage2Buffer->setSize(numChannels, samplesPerBlock);
	mStage2InputGainPtr->prepare(spec);
	mStage2WaveShaperPtr->prepare(spec);
	mStage2OutputGainPtr->prepare(spec);
	mStage2DryWetMixerPtr->prepare(spec);

	mStage3Buffer->setSize(numChannels, samplesPerBlock);
	mStage3InputGainPtr->prepare(spec);
	mStage3WaveShaperPtr->prepare(spec);
	mStage3OutputGainPtr->prepare(spec);
	mStage3DryWetMixerPtr->prepare(spec);

	mStage4Buffer->setSize(numChannels, samplesPerBlock);
	mStage4InputGainPtr->prepare(spec);
	mStage4WaveShaperPtr->prepare(spec);
	mStage4OutputGainPtr->prepare(spec);
	mStage4DryWetMixerPtr->prepare(spec);

	mBiasPtr->prepare(spec);

	mPostCompressorPtr->prepare(spec);
	mCompressorGainPtr->prepare(spec);

	mHighPassFilterPtr->prepare(spec);
	mMidPeakFilterPtr->prepare(spec);
	mHighShelfFilterPtr->prepare(spec);
	mLowPassFilterPtr->prepare(spec);

	mDelayLinePtr->prepare(spec);
	mDelayLinePtr->setMaximumDelayInSamples(apvts::delayTimeMsMaximumValue * (spec.sampleRate / 1000));
	mDelayLineDryWetMixerPtr->prepare(spec);

	mChorusPtr->prepare(spec);
	mPhaserPtr->prepare(spec);

	mCabinetImpulseResponseConvolutionPtr->prepare(spec);
	loadImpulseResponseFromState();

	mReverb->prepare(spec);

	mCabinetGainPtr->prepare(spec);
	mLimiter->prepare(spec);

	mOutputGainPtr->prepare(spec);

	for (const auto& patameterIdToEnum : apvts::parameterIdToEnumMap)
	{
		auto newValue = mAudioProcessorValueTreeStatePtr->getParameterAsValue(patameterIdToEnum.first).getValue();
		parameterChanged(patameterIdToEnum.first, newValue);
	}
}

void PluginAudioProcessor::reset()
{
	mNoiseGate->reset();
	mPreCompressorPtr->reset();

	mTubeScreamerPtr->reset();
	mMouseDrivePtr->reset();

	mStage1InputGainPtr->reset();
	mStage1WaveShaperPtr->reset();
	mStage1OutputGainPtr->reset();
	mStage1DryWetMixerPtr->reset();

	mStage2InputGainPtr->reset();
	mStage2WaveShaperPtr->reset();
	mStage2OutputGainPtr->reset();
	mStage2DryWetMixerPtr->reset();

	mStage3InputGainPtr->reset();
	mStage3WaveShaperPtr->reset();
	mStage3OutputGainPtr->reset();
	mStage3DryWetMixerPtr->reset();

	mStage4InputGainPtr->reset();
	mStage4WaveShaperPtr->reset();
	mStage4OutputGainPtr->reset();
	mStage4DryWetMixerPtr->reset();

	mBiasPtr->reset();
	mPostCompressorPtr->reset();
	
	mHighPassFilterPtr->reset();
	mMidPeakFilterPtr->reset();
	mHighShelfFilterPtr->reset();
	mLowPassFilterPtr->reset();
	
	mCompressorGainPtr->reset();

	mDelayLinePtr->reset();
	mDelayLineDryWetMixerPtr->reset();

	mChorusPtr->reset();
	mPhaserPtr->reset();

	mCabinetImpulseResponseConvolutionPtr->reset();

	mReverb->reset();

	mCabinetGainPtr->reset();

	mLimiter->reset();

	mOutputGainPtr->reset();
}

void PluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	const auto totalNumInputChannels = getTotalNumInputChannels();
	const auto totalNumOutputChannels = getTotalNumOutputChannels();
	const auto numSamples = buffer.getNumSamples();
	const auto samplesPerSecond = getSampleRate();
	const double rawBeatsPerMinute = getPlayHead()->getPosition()->getBpm().orFallback(120);
	mBpmSmoothedValue.setTargetValue(rawBeatsPerMinute);
	const auto smoothedBeatsPerMinute = mBpmSmoothedValue.skip(numSamples);

	if (mBypassIsOn)
	{
		return;
	}

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
		buffer.clear(i, 0, numSamples);
	}

	auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
	auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

	mNoiseGate->process(processContext);
	mPreCompressorPtr->process(processContext);

	if (mTubeScreamerIsOn)
	{
		mTubeScreamerPtr->processBlock(buffer);
	}

	if (mMouseDriveIsOn)
	{
		mMouseDrivePtr->processBlock(buffer);
	}

	mStage1Buffer->clear();
	auto stage1Block = juce::dsp::AudioBlock<float>(*mStage1Buffer.get());
	
	if (mStagesAreParallel)
	{
		stage1Block.copyFrom(buffer, 0, 0, numSamples);
	}
	
	auto stage1Context = juce::dsp::ProcessContextReplacing<float>(stage1Block);
	if (mStage1IsOn)
	{
		mStage1DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mStagesAreParallel)
		{
			mStage1InputGainPtr->process(stage1Context);
			mStage1WaveShaperPtr->process(stage1Context);
			mStage1OutputGainPtr->process(stage1Context);
			mStage1DryWetMixerPtr->mixWetSamples(stage1Block);
		}
		else
		{
			mStage1InputGainPtr->process(processContext);
			mStage1WaveShaperPtr->process(processContext);
			mStage1OutputGainPtr->process(processContext);
			mStage1DryWetMixerPtr->mixWetSamples(audioBlock);
		}
	}

	mStage2Buffer->clear();
	auto stage2Block = juce::dsp::AudioBlock<float>(*mStage2Buffer.get());
	if (mStagesAreParallel)
	{
		stage2Block.copyFrom(buffer, 0, 0, numSamples);
	}
	auto stage2Context = juce::dsp::ProcessContextReplacing<float>(stage2Block);

	if (mStage2IsOn)
	{
		mStage2DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mStagesAreParallel)
		{
			mStage2InputGainPtr->process(stage2Context);
			mStage2WaveShaperPtr->process(stage2Context);
			mStage2OutputGainPtr->process(stage2Context);
			mStage2DryWetMixerPtr->mixWetSamples(stage2Block);
		}
		else
		{
			mStage2InputGainPtr->process(processContext);
			mStage2WaveShaperPtr->process(processContext);
			mStage2OutputGainPtr->process(processContext);
			mStage2DryWetMixerPtr->mixWetSamples(audioBlock);
		}
	}

	mStage3Buffer->clear();
	auto stage3Block = juce::dsp::AudioBlock<float>(*mStage3Buffer.get());
	if (mStagesAreParallel)
	{
		stage3Block.copyFrom(buffer, 0, 0, numSamples);
	}
	auto stage3Context = juce::dsp::ProcessContextReplacing<float>(stage3Block);

	if (mStage3IsOn)
	{
		mStage3DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mStagesAreParallel)
		{
			mStage3InputGainPtr->process(stage3Context);
			mStage3WaveShaperPtr->process(stage3Context);
			mStage3OutputGainPtr->process(stage3Context);
			mStage3DryWetMixerPtr->mixWetSamples(stage3Block);
		}
		else
		{
			mStage3InputGainPtr->process(processContext);
			mStage3WaveShaperPtr->process(processContext);
			mStage3OutputGainPtr->process(processContext);
			mStage3DryWetMixerPtr->mixWetSamples(audioBlock);
		}
	}

	mStage4Buffer->clear();
	auto stage4Block = juce::dsp::AudioBlock<float>(*mStage4Buffer.get());
	if (mStagesAreParallel)
	{
		stage4Block.copyFrom(buffer, 0, 0, numSamples);
	}
	auto stage4Context = juce::dsp::ProcessContextReplacing<float>(stage4Block);

	if (mStage4IsOn)
	{
		mStage4DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mStagesAreParallel)
		{
			mStage4InputGainPtr->process(stage4Context);
			mStage4WaveShaperPtr->process(stage4Context);
			mStage4OutputGainPtr->process(stage4Context);
			mStage4DryWetMixerPtr->mixWetSamples(stage4Block);
		}
		else
		{
			mStage4InputGainPtr->process(processContext);
			mStage4WaveShaperPtr->process(processContext);
			mStage4OutputGainPtr->process(processContext);
			mStage4DryWetMixerPtr->mixWetSamples(audioBlock);
		}
	}

	if ((!mStage1IsOn && !mStage2IsOn && !mStage3IsOn && !mStage4IsOn) || mStagesAreParallel)
	{
		audioBlock.clear();
	}

	if (mStage1IsOn && mStagesAreParallel)
	{
		audioBlock.add(stage1Block);
	}

	if (mStage2IsOn && mStagesAreParallel)
	{
		audioBlock.add(stage2Block);
	}

	if (mStage3IsOn && mStagesAreParallel)
	{
		audioBlock.add(stage3Block);
	}

	if (mStage4IsOn && mStagesAreParallel)
	{
		audioBlock.add(stage4Block);
	}

	mBiasPtr->process(processContext);

	mPostCompressorPtr->process(processContext);
	mCompressorGainPtr->process(processContext);
	mHighPassFilterPtr->process(processContext);
	mMidPeakFilterPtr->process(processContext);
	mHighShelfFilterPtr->process(processContext);
	mLowPassFilterPtr->process(processContext);

	mDelayLineDryWetMixerPtr->pushDrySamples(audioBlock);

	if (mDelayFeedback > 0.0f)
	{
		float fractionOfWholeBeat = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::delayTimeFractionalDenominatorId);
		const auto nextDelaySamples = apvts::calculateSamplesForBpmFractionAndRate(smoothedBeatsPerMinute, fractionOfWholeBeat, samplesPerSecond);

		if (mDelayLinePtr->getDelay() != nextDelaySamples)
		{
			mDelayLinePtr->setDelay(nextDelaySamples);
		}

		for (int channelIndex = 0; channelIndex < getTotalNumInputChannels(); ++channelIndex)
		{
			auto* channelData = audioBlock.getChannelPointer(channelIndex);

			if (channelData)
			{
				for (int i = 0; i < numSamples; ++i)
				{
					const float delayedSample = mDelayLinePtr->popSample(channelIndex, mDelayLinePtr->getDelay());

					if (i < audioBlock.getNumSamples())
					{
						mDelayLinePtr->pushSample(channelIndex, channelData[i] + mDelayFeedback * delayedSample);
						channelData[i] += delayedSample;
					}
				}
			}
		}
	}
	
	
	mDelayLineDryWetMixerPtr->mixWetSamples(audioBlock);
	
	float chorusFractionOfBeat = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::chorusFractionOfBeatId);
	float chorusSmoothedRate = apvts::clampedValueForFractionOfBeat(smoothedBeatsPerMinute, chorusFractionOfBeat);
	mChorusPtr->setRate(chorusSmoothedRate);

	float chorusDelayFractionOfBeat = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::chorusCenterDelayFractionOfBeatId);
	float chorusSmoothedDelay = apvts::clampedValueForFractionOfBeat(smoothedBeatsPerMinute, chorusDelayFractionOfBeat);
	mChorusPtr->setCentreDelay(chorusSmoothedDelay);

	mChorusPtr->process(processContext);

	float phaserFractionOfBeat = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::phaserRateFractionOfBeatId);
	float phaserSmoothedRate = apvts::clampedValueForFractionOfBeat(smoothedBeatsPerMinute, phaserFractionOfBeat);
	mPhaserPtr->setRate(phaserSmoothedRate);

	mPhaserPtr->process(processContext);

	if (mCabImpulseResponseConvolutionIsOn)
	{
		mCabinetImpulseResponseConvolutionPtr->process(processContext);
		mCabinetGainPtr->process(processContext);
	}

	if (mReverbOn)
	{
		mReverb->process(processContext);
	}
	
	if (mLimiterOn)
	{
		mLimiter->process(processContext);
	}

	mOutputGainPtr->process(processContext);
}

void PluginAudioProcessor::parameterChanged(const juce::String& parameterIdJuceString, float newValue)
{
	auto sampleRate = getSampleRate();
	double beatsPerMinute = getPlayHead()->getPosition()->getBpm().orFallback(120);

	switch (apvts::parameterIdToEnumMap.at(parameterIdJuceString.toStdString()))
	{
	case apvts::ParameterEnum::REVERB_ON:
		mReverbOn = newValue;
		break;
	case apvts::ParameterEnum::LIMITER_ON:
		mLimiterOn = newValue;
		break;
	case apvts::ParameterEnum::BYPASS_ON:
		mBypassIsOn = newValue;
		break;
	case apvts::ParameterEnum::STAGE1_ON:
		mStage1IsOn = newValue;
		break;
	case apvts::ParameterEnum::STAGE2_ON:
		mStage2IsOn = newValue;
		break;
	case apvts::ParameterEnum::STAGE3_ON:
		mStage3IsOn = newValue;
		break;
	case apvts::ParameterEnum::STAGE4_ON:
		mStage4IsOn = newValue;
		break;
	case apvts::ParameterEnum::STAGE1_INPUT_GAIN:
		mStage1InputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE2_INPUT_GAIN:
		mStage2InputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE3_INPUT_GAIN:
		mStage3InputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE4_INPUT_GAIN:
		mStage4InputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE1_WAVE_SHAPER:
		mStage1WaveShaperPtr->functionToUse = apvts::waveShaperIdToFunctionMap.at(apvts::waveShaperIds.at(newValue));
		break;
	case apvts::ParameterEnum::STAGE2_WAVE_SHAPER:
		mStage2WaveShaperPtr->functionToUse = apvts::waveShaperIdToFunctionMap.at(apvts::waveShaperIds.at(newValue));
		break;
	case apvts::ParameterEnum::STAGE3_WAVE_SHAPER:
		mStage3WaveShaperPtr->functionToUse = apvts::waveShaperIdToFunctionMap.at(apvts::waveShaperIds.at(newValue));
		break;
	case apvts::ParameterEnum::STAGE4_WAVE_SHAPER:
		mStage4WaveShaperPtr->functionToUse = apvts::waveShaperIdToFunctionMap.at(apvts::waveShaperIds.at(newValue));
		break;
	case apvts::ParameterEnum::STAGE1_OUTPUT_GAIN:
		mStage1OutputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE2_OUTPUT_GAIN:
		mStage2OutputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE3_OUTPUT_GAIN:
		mStage3OutputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE4_OUTPUT_GAIN:
		mStage4OutputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::STAGE1_DRY_WET:
		mStage1DryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::STAGE2_DRY_WET:
		mStage2DryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::STAGE3_DRY_WET:
		mStage3DryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::STAGE4_DRY_WET:
		mStage4DryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::STAGE_MODE:
		mStagesAreParallel = newValue;
		break;
	case apvts::ParameterEnum::BIAS:
		mBiasPtr->setBias(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_THRESHOLD:
		mPostCompressorPtr->setThreshold(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_ATTACK:
		mPostCompressorPtr->setAttack(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_RATIO:
		mPostCompressorPtr->setRatio(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_RELEASE:
		mPostCompressorPtr->setRelease(newValue);
		break;
	case apvts::ParameterEnum::PRE_COMPRESSOR_THRESHOLD:
		mPreCompressorPtr->setThreshold(newValue);
		break;
	case apvts::ParameterEnum::PRE_COMPRESSOR_ATTACK:
		mPreCompressorPtr->setAttack(newValue);
		break;
	case apvts::ParameterEnum::PRE_COMPRESSOR_RATIO:
		mPreCompressorPtr->setRatio(newValue);
		break;
	case apvts::ParameterEnum::PRE_COMPRESSOR_RELEASE:
		mPreCompressorPtr->setRelease(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_GAIN:
		mCompressorGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::HIGH_PASS_FREQUENCY:
	{
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highPassQId).getValue();
		*mHighPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
			sampleRate,
			std::max(newValue, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue));
	}
	break;
	case apvts::ParameterEnum::HIGH_PASS_Q:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highPassFrequencyId).getValue();
		*mHighPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(newValue, apvts::qualityMinimumValue));
	}
	break;
	case apvts::ParameterEnum::LOW_PASS_FREQUENCY:
	{
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::lowPassQId).getValue();
		*mLowPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
			sampleRate,
			std::max(newValue, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue));
	}
	break;
	case apvts::ParameterEnum::LOW_PASS_Q:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::lowPassFrequencyId).getValue();
		*mLowPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(newValue, apvts::qualityMinimumValue));
	}
	break;
	case apvts::ParameterEnum::MID_PEAK_FREQUENCY:
	{
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakGainId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakQId).getValue();
		*mMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
			sampleRate,
			std::max(newValue, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			std::max(gain, apvts::gainLinearMinimumValue));
	}
	break;
	case apvts::ParameterEnum::MID_PEAK_Q:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakFrequencyId).getValue();
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakGainId).getValue();
		*mMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(newValue, apvts::qualityMinimumValue),
			std::max(gain, apvts::gainLinearMinimumValue));
	}
	break;
	case apvts::ParameterEnum::MID_PEAK_GAIN:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakFrequencyId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakQId).getValue();
		*mMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			newValue);
	}
	break;
	case apvts::ParameterEnum::HIGH_SHELF_FREQUENCY:
	{
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfGainId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfQId).getValue();
		*mHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
			sampleRate,
			std::max(newValue, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			gain);
	}
	break;
	case apvts::ParameterEnum::HIGH_SHELF_Q:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfFrequencyId).getValue();
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfGainId).getValue();
		*mHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(newValue, apvts::qualityMinimumValue),
			gain);
	}
	break;
	case apvts::ParameterEnum::HIGH_SHELF_GAIN:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfFrequencyId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfQId).getValue();
		*mHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			newValue);
	}
	break;
	case apvts::ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON:
		mCabImpulseResponseConvolutionIsOn = newValue;
		break;
	case apvts::ParameterEnum::CABINET_OUTPUT_GAIN:
		mCabinetGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::LIMITER_RELEASE:
		mLimiter->setRelease(newValue);
		break;
	case apvts::ParameterEnum::LIMITER_THRESHOLD:
		mLimiter->setThreshold(newValue);
		break;
	case apvts::ParameterEnum::CHORUS_FRACTION_OF_BEAT:
		mChorusPtr->setRate(apvts::clampedValueForFractionOfBeat(beatsPerMinute, newValue));
		break;
	case apvts::ParameterEnum::CHORUS_DEPTH:
		mChorusPtr->setDepth(newValue);
		break;
	case apvts::ParameterEnum::CHORUS_CENTER_DELAY_FRACTION_OF_BEAT:
		mChorusPtr->setCentreDelay(apvts::clampedValueForFractionOfBeat(beatsPerMinute, newValue));
		break;
	case apvts::ParameterEnum::CHORUS_FEEDBACK:
		mChorusPtr->setFeedback(newValue);
		break;
	case apvts::ParameterEnum::CHORUS_MIX:
		mChorusPtr->setMix(newValue);
		break;
	case apvts::ParameterEnum::PHASER_RATE_FRACTION_OF_BEAT:
		mPhaserPtr->setRate(apvts::clampedValueForFractionOfBeat(beatsPerMinute, newValue));
		break;
	case apvts::ParameterEnum::PHASER_DEPTH:
		mPhaserPtr->setDepth(newValue);
		break;
	case apvts::ParameterEnum::PHASER_CENTER_FREQUENCY:
		mPhaserPtr->setCentreFrequency(newValue);
		break;
	case apvts::ParameterEnum::PHASER_FEEDBACK:
		mPhaserPtr->setFeedback(newValue);
		break;
	case apvts::ParameterEnum::PHASER_MIX:
		mPhaserPtr->setMix(newValue);
		break;
	case apvts::ParameterEnum::REVERB_SIZE:
	{
		const auto& parameters = mReverb->getParameters();
		mReverb->setParameters({
			newValue,
			parameters.damping,
			parameters.wetLevel,
			parameters.dryLevel,
			parameters.width,
			0.0f });
	}
		break;
	case apvts::ParameterEnum::REVERB_DAMPING:
	{
		const auto& parameters = mReverb->getParameters();
		mReverb->setParameters({
			parameters.roomSize,
			newValue,
			parameters.wetLevel,
			parameters.dryLevel,
			parameters.width,
			0.0f });
		}
		break;
	case apvts::ParameterEnum::REVERB_MIX:
	{
		float wetLevelLinear = newValue;
		float dryLevelLinear = 1.0f - newValue;

		float wetLevelGain = pow(wetLevelLinear, 2.0f); 
		float dryLevelGain = fmax(sqrt(1.0f - pow(wetLevelLinear, 2.0f)), 0.001);

		const auto& parameters = mReverb->getParameters();
		mReverb->setParameters({
			parameters.roomSize,
			parameters.damping,
			wetLevelGain, // wet
			dryLevelGain, // dry
			parameters.width,
			0.0f });
	}
		break;
	case apvts::ParameterEnum::REVERB_WIDTH:
	{
		const auto& parameters = mReverb->getParameters();
		mReverb->setParameters({
			parameters.roomSize,
			parameters.damping,
			parameters.wetLevel,
			parameters.dryLevel,
			newValue,
			0.0f });
	}
		break;
	case apvts::ParameterEnum::DELAY_DRY_WET:
		mDelayLineDryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::DELAY_FEEDBACK:
		mDelayFeedback = newValue;
		break;
	case apvts::ParameterEnum::DELAY_TIME_FRACTIONAL_DENOMINATOR:
		mDelayLinePtr->setDelay(apvts::calculateSamplesForBpmFractionAndRate(beatsPerMinute, newValue, sampleRate));
		break;
	case apvts::ParameterEnum::NOISE_GATE_THRESHOLD:
		mNoiseGate->setThreshold(newValue);
		break;
	case apvts::ParameterEnum::NOISE_GATE_ATTACK:
		mNoiseGate->setAttack(newValue);
		break;
	case apvts::ParameterEnum::NOISE_GATE_RATIO:
		mNoiseGate->setRatio(newValue);
		break;
	case apvts::ParameterEnum::NOISE_GATE_RELEASE:
		mNoiseGate->setRelease(newValue);
		break;
	case apvts::ParameterEnum::MOUSE_DRIVE_DISTORTION:
		mMouseDrivePtr->setDistortion(newValue);
		break;
	case apvts::ParameterEnum::MOUSE_DRIVE_VOLUME:
		mMouseDrivePtr->setVolume(newValue);
		break;
	case apvts::ParameterEnum::MOUSE_DRIVE_ON:
		mMouseDriveIsOn = newValue;
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_ON:
		mTubeScreamerIsOn = newValue;
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_DRIVE:
		mTubeScreamerPtr->setDrive(newValue);
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_LEVEL:
		mTubeScreamerPtr->setLevel(newValue);
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_DIODE_TYPE:
		mTubeScreamerPtr->setDiodeType(newValue);
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_DIODE_COUNT:
		mTubeScreamerPtr->setDiodeCount(newValue);
		break;
	case apvts::ParameterEnum::OUTPUT_GAIN:
		mOutputGainPtr->setGainDecibels(newValue);
		break;
	default:
		assert(false);
		}
}

void PluginAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) 
{
	if (property == juce::Identifier(apvts::impulseResponseFileFullPathNameId))
	{
		loadImpulseResponseFromState();
	}
}

void PluginAudioProcessor::loadImpulseResponseFromState()
{
	const auto impulseResponseFullPathName = mAudioProcessorValueTreeStatePtr->state.getProperty(
		juce::String(apvts::impulseResponseFileFullPathNameId),
		juce::String()).toString();
	juce::File* impulseResponseFile = &juce::File(impulseResponseFullPathName);

	if (impulseResponseFullPathName.length() > 0)
	{
		mCabinetImpulseResponseConvolutionPtr->loadImpulseResponse(
			*impulseResponseFile, juce::dsp::Convolution::Stereo::yes,
			juce::dsp::Convolution::Trim::no, 0,
			juce::dsp::Convolution::Normalise::yes
		);
	}
	else
	{
		mCabinetImpulseResponseConvolutionPtr->loadImpulseResponse(
			BinaryData::cory_bread_and_butter_normalized_wav,
			BinaryData::cory_bread_and_butter_normalized_wavSize,
			juce::dsp::Convolution::Stereo::yes,
			juce::dsp::Convolution::Trim::no,
			BinaryData::cory_bread_and_butter_normalized_wavSize,
			juce::dsp::Convolution::Normalise::yes);
	}
}

void PluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	auto state = mAudioProcessorValueTreeStatePtr->copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void PluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName(mAudioProcessorValueTreeStatePtr->state.getType()))
		{
			const auto valueTree = juce::ValueTree::fromXml(*xmlState);
			mAudioProcessorValueTreeStatePtr->replaceState(valueTree);
			loadImpulseResponseFromState();
		}
	}
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new PluginAudioProcessor();
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

const juce::String PluginAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool PluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool PluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
	return 1;
}

int PluginAudioProcessor::getCurrentProgram()
{
	return 0;
}

void PluginAudioProcessor::setCurrentProgram(int index)
{
	juce::ignoreUnused(index);
}

const juce::String PluginAudioProcessor::getProgramName(int index)
{
	return {};
}

void PluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
	ignoreUnused(index, newName);
}

void PluginAudioProcessor::releaseResources()
{

}

bool PluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

#if !JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}

bool PluginAudioProcessor::hasEditor() const
{
	return true;
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
	return new PluginAudioProcessorEditor(
		*this,
		*mAudioProcessorValueTreeStatePtr.get(),
		*mPresetManagerPtr.get()
	);
}
