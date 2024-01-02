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
	mAmpCompressorPtr(std::make_unique<juce::dsp::Compressor<float>>()),
	mAmpCompressorGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mAmpHighPassFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mAmpMidPeakFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mAmpHighShelfFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mConvolutionMessageQueuePtr(std::make_unique<juce::dsp::ConvolutionMessageQueue>()),
	mCabinetImpulseResponseConvolutionPtr(std::make_unique<juce::dsp::Convolution>(juce::dsp::Convolution::NonUniform{ 2048 }, * mConvolutionMessageQueuePtr.get())),
	mOutputGainPtr(std::make_unique<juce::dsp::Gain<float>>())
{
	mAudioFormatManagerPtr->registerBasicFormats();

	mOutputGainPtr->setGainDecibels(apvts::gainDefaultValue);
	mAmpCompressorGainPtr->setGainDecibels(apvts::gainDefaultValue);

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
	for (const auto& item : apvts::modeIds) {
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
		case apvts::ParameterEnum::STAGE2_ON:
		case apvts::ParameterEnum::STAGE3_ON:
		case apvts::ParameterEnum::STAGE4_ON:
		case apvts::ParameterEnum::HIGH_PASS_ON:
		case apvts::ParameterEnum::MID_PEAK_ON:
		case apvts::ParameterEnum::HIGH_SHELF_ON:
		case apvts::ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON:
			layout.add(std::make_unique<juce::AudioParameterBool>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				false
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
		case apvts::ParameterEnum::COMPRESSOR_GAIN:
		case apvts::ParameterEnum::OUTPUT_GAIN:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::gainNormalizableRange,
				apvts::gainDefaultValue
				));
			break;
		case apvts::ParameterEnum::STAGE1_WAVE_SHAPER:
		case apvts::ParameterEnum::STAGE2_WAVE_SHAPER:
		case apvts::ParameterEnum::STAGE3_WAVE_SHAPER:
		case apvts::ParameterEnum::STAGE4_WAVE_SHAPER:
			layout.add(std::make_unique<juce::AudioParameterChoice>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				waveShaperIdsJuceStringArray,
				0));
			break;

		case apvts::ParameterEnum::STAGE1_DRY_WET:
		case apvts::ParameterEnum::STAGE2_DRY_WET:
		case apvts::ParameterEnum::STAGE3_DRY_WET:
		case apvts::ParameterEnum::STAGE4_DRY_WET:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::dryWetNormalizableRange,
				apvts::dryWetDefaultValue
				));
			break;
		case apvts::ParameterEnum::MODE:
			layout.add(std::make_unique<juce::AudioParameterChoice>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				modeIdsJuceStringArray,
				0));
			break;
		case apvts::ParameterEnum::BIAS:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::biasNormalizableRange,
				apvts::biasDefaultValue
				));
			break;
		case apvts::ParameterEnum::COMPRESSOR_THRESHOLD:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::thresholdNormalizableRange,
				apvts::thresholdDefaultValue
				));
			break;
		case apvts::ParameterEnum::COMPRESSOR_ATTACK:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::attackNormalizableRange,
				apvts::attackDefaultValue
				));
			break;
		case apvts::ParameterEnum::COMPRESSOR_RATIO:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::ratioNormalizableRange,
				apvts::ratioDefaultValue
				));
			break;
		case apvts::ParameterEnum::COMPRESSOR_RELEASE:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::releaseNormalizableRange,
				apvts::releaseDefaultValue
				));
			break;
		case apvts::ParameterEnum::HIGH_PASS_FREQUENCY:
		case apvts::ParameterEnum::MID_PEAK_FREQUENCY:
		case apvts::ParameterEnum::HIGH_SHELF_FREQUENCY:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::frequencyNormalizableRange,
				apvts::equalizationTypeIdToDefaultFrequencyMap.at(parts[0])
				));
			break;
		case apvts::ParameterEnum::HIGH_PASS_Q:
		case apvts::ParameterEnum::MID_PEAK_Q:
		case apvts::ParameterEnum::HIGH_SHELF_Q:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::qualityNormalizableRange,
				apvts::qualityDefaultValue
				));
			break;
		case apvts::ParameterEnum::MID_PEAK_GAIN:
		case apvts::ParameterEnum::HIGH_SHELF_GAIN:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				parameterId,
				apvts::eqGainNormalizableRange,
				apvts::eqGainDefaultValue
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

	mAmpCompressorPtr->prepare(spec);
	mAmpCompressorGainPtr->prepare(spec);

	mAmpHighPassFilterPtr->prepare(spec);
	mAmpMidPeakFilterPtr->prepare(spec);
	mAmpHighShelfFilterPtr->prepare(spec);

	mCabinetImpulseResponseConvolutionPtr->prepare(spec);
	mCabinetImpulseResponseConvolutionPtr->loadImpulseResponse(
		BinaryData::cory_bread_and_butter_normalized_wav,
		BinaryData::cory_bread_and_butter_normalized_wavSize,
		juce::dsp::Convolution::Stereo::yes,
		juce::dsp::Convolution::Trim::no,
		BinaryData::cory_bread_and_butter_normalized_wavSize,
		juce::dsp::Convolution::Normalise::yes);

	mOutputGainPtr->prepare(spec);

	for (const auto& patameterIdToEnum : apvts::parameterIdToEnumMap)
	{
		auto newValue = mAudioProcessorValueTreeStatePtr->getParameterAsValue(patameterIdToEnum.first).getValue();
		parameterChanged(patameterIdToEnum.first, newValue);
	}
}

void PluginAudioProcessor::reset()
{
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

	mBiasPtr->reset();
	mAmpCompressorPtr->reset();
	mAmpHighPassFilterPtr->reset();
	mAmpMidPeakFilterPtr->reset();
	mAmpHighShelfFilterPtr->reset();
	mAmpCompressorGainPtr->reset();
	mCabinetImpulseResponseConvolutionPtr->reset();
	mOutputGainPtr->reset();
}

void PluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	auto numSamples = buffer.getNumSamples();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
		buffer.clear(i, 0, numSamples);
	}

	auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
	auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

	mStage1Buffer->clear();
	auto stage1Block = juce::dsp::AudioBlock<float>(*mStage1Buffer.get());
	if (mParallel)
	{
		stage1Block.copyFrom(buffer, 0, 0, numSamples);
	}
	auto stage1Context = juce::dsp::ProcessContextReplacing<float>(stage1Block);

	if (mStage1On)
	{	
		mStage1DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mParallel)
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
	if (mParallel)
	{
		stage2Block.copyFrom(buffer, 0, 0, numSamples);
	}
	auto stage2Context = juce::dsp::ProcessContextReplacing<float>(stage2Block);

	if (mStage2On)
	{
		mStage2DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mParallel)
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
	if (mParallel)
	{
		stage3Block.copyFrom(buffer, 0, 0, numSamples);
	}
	auto stage3Context = juce::dsp::ProcessContextReplacing<float>(stage3Block);

	if (mStage3On)
	{
		mStage3DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mParallel)
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
	if (mParallel)
	{
		stage4Block.copyFrom(buffer, 0, 0, numSamples);
	}
	auto stage4Context = juce::dsp::ProcessContextReplacing<float>(stage4Block);

	if (mStage4On)
	{
		mStage4DryWetMixerPtr->pushDrySamples(audioBlock);

		if (mParallel)
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

	if (!mStage1On && !mStage2On && !mStage3On && !mStage4On || mParallel)
	{
		audioBlock.clear();
	}

	if (mStage1On && mParallel)
	{
		audioBlock.add(stage1Block);
	}

	if (mStage2On && mParallel)
	{
		audioBlock.add(stage2Block);
	}

	if (mStage3On && mParallel)
	{
		audioBlock.add(stage3Block);
	}

	if (mStage4On && mParallel)
	{
		audioBlock.add(stage4Block);
	}

	mBiasPtr->process(processContext);

	mAmpCompressorPtr->process(processContext);

	if (mAmpHighPassFilterOn)
	{
		mAmpHighPassFilterPtr->process(processContext);
	}

	if (mAmpMidPeakFilterOn)
	{
		mAmpMidPeakFilterPtr->process(processContext);
	}

	if (mAmpHighShelfFilterOn)
	{
		mAmpHighShelfFilterPtr->process(processContext);
	}

	mAmpCompressorGainPtr->process(processContext);

	if (mCabImpulseResponseConvolutionOn)
	{
		mCabinetImpulseResponseConvolutionPtr->process(processContext);
	}

	mOutputGainPtr->process(processContext);
}

void PluginAudioProcessor::parameterChanged(const juce::String& parameterIdJuceString, float newValue)
{
	auto sampleRate = getSampleRate();

	std::string parameterId = parameterIdJuceString.toStdString();

	std::vector<std::string> parts;
	std::stringstream ss(parameterId);
	std::string item;

	while (std::getline(ss, item, '_')) {
		parts.push_back(item);
	}

	switch (apvts::parameterIdToEnumMap.at(parameterId))
	{
	case apvts::ParameterEnum::STAGE1_ON:
		mStage1On = newValue;
		break;
	case apvts::ParameterEnum::STAGE2_ON:
		mStage2On = newValue;
		break;
	case apvts::ParameterEnum::STAGE3_ON:
		mStage3On = newValue;
		break;
	case apvts::ParameterEnum::STAGE4_ON:
		mStage4On = newValue;
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
	case apvts::ParameterEnum::MODE:
		mParallel = newValue;
		break;
	case apvts::ParameterEnum::BIAS:
		mBiasPtr->setBias(newValue);
		break;
	case apvts::ParameterEnum::COMPRESSOR_THRESHOLD:
		mAmpCompressorPtr->setThreshold(newValue);
		break;
	case apvts::ParameterEnum::COMPRESSOR_ATTACK:
		mAmpCompressorPtr->setAttack(newValue);
		break;
	case apvts::ParameterEnum::COMPRESSOR_RATIO:
		mAmpCompressorPtr->setRatio(newValue);
		break;
	case apvts::ParameterEnum::COMPRESSOR_RELEASE:
		mAmpCompressorPtr->setRelease(newValue);
		break;
	case apvts::ParameterEnum::COMPRESSOR_GAIN:
		mAmpCompressorGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::HIGH_PASS_ON:
		mAmpHighPassFilterOn = newValue;
		break;
	case apvts::ParameterEnum::HIGH_PASS_FREQUENCY:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highPassFrequencyId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highPassQId).getValue();
		*mAmpHighPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
			sampleRate,
			std::max(newValue, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue));
	}
	break;
	case apvts::ParameterEnum::HIGH_PASS_Q:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highPassFrequencyId).getValue();
		*mAmpHighPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(newValue, apvts::qualityMinimumValue));
	}
	break;
	case apvts::ParameterEnum::MID_PEAK_ON:
		mAmpMidPeakFilterOn = newValue;
		break;
	case apvts::ParameterEnum::MID_PEAK_FREQUENCY:
	{
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakGainId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakQId).getValue();
		*mAmpMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
			sampleRate,
			std::max(newValue, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			juce::Decibels::decibelsToGain(gain));
	}
		break;
	case apvts::ParameterEnum::MID_PEAK_Q:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakFrequencyId).getValue();
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakGainId).getValue();
		*mAmpMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(newValue, apvts::qualityMinimumValue),
			juce::Decibels::decibelsToGain(gain));
	}
		break;
	case apvts::ParameterEnum::MID_PEAK_GAIN:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakFrequencyId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::midPeakQId).getValue();
		*mAmpMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			juce::Decibels::decibelsToGain(newValue));
	}
		break;
	case apvts::ParameterEnum::HIGH_SHELF_ON:
		mAmpHighShelfFilterOn = newValue;
		break;
	case apvts::ParameterEnum::HIGH_SHELF_FREQUENCY:
	{
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfGainId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfQId).getValue();
		*mAmpHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
			sampleRate,
			std::max(newValue, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			juce::Decibels::decibelsToGain(gain));
	}
		break;
	case apvts::ParameterEnum::HIGH_SHELF_Q:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfFrequencyId).getValue();
		const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfGainId).getValue();
		*mAmpHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(newValue, apvts::qualityMinimumValue),
			juce::Decibels::decibelsToGain(gain));
	}
		break;
	case apvts::ParameterEnum::HIGH_SHELF_GAIN:
	{
		const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfFrequencyId).getValue();
		const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::highShelfQId).getValue();
		*mAmpHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
			sampleRate,
			std::max(frequency, apvts::frequencyMinimumValue),
			std::max(quality, apvts::qualityMinimumValue),
			juce::Decibels::decibelsToGain(newValue));
	}
		break;
	case apvts::ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON:
		mCabImpulseResponseConvolutionOn = newValue;
		break;
	case apvts::ParameterEnum::OUTPUT_GAIN:
		mOutputGainPtr->setGainDecibels(newValue);
		break;
	default:
		assert(false);
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
			mAudioProcessorValueTreeStatePtr->replaceState(juce::ValueTree::fromXml(*xmlState));
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
