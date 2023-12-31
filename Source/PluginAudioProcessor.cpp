#include "PluginAudioProcessor.h"
#include "BinaryData.h"
#include "PluginAudioProcessorEditor.h"
#include "PluginAudioParameters.h"
#include <cassert>

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
	mAudioFormatManagerPtr(std::make_unique<juce::AudioFormatManager>()),
	mInputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mOverdriveTanhWaveShaperPtr(std::make_unique<juce::dsp::WaveShaper<float>>()),
	mOverdriveSoftClipWaveShaperPtr(std::make_unique<juce::dsp::WaveShaper<float>>()),
	mOverdriveBiasPtr(std::make_unique<juce::dsp::Bias<float>>()),
	mOverdriveGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mQueue(std::make_unique<juce::dsp::ConvolutionMessageQueue>()),
	mAmpImpulseResponseConvolutionPtr(std::make_unique<juce::dsp::Convolution>(juce::dsp::Convolution::NonUniform{ 2048 }, *mQueue.get())),
	mAmpCompressorPtr(std::make_unique<juce::dsp::Compressor<float>>()),
	mAmpHighPassFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mAmpMidPeakFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mAmpHighShelfFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>()),
	mAmpGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mCabinetImpulseResponseConvolutionPtr(std::make_unique<juce::dsp::Convolution>(juce::dsp::Convolution::NonUniform{ 2048 }, *mQueue.get())),
	mOutputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mPresetManagerPtr(std::make_unique<PluginPresetManager>(*mAudioProcessorValueTreeStatePtr.get()))
{
	mAudioFormatManagerPtr->registerBasicFormats();

	mInputGainPtr->setGainDecibels(apvts::gainDefaultValue);
	mAmpGainPtr->setGainDecibels(apvts::gainDefaultValue);
	mOverdriveGainPtr->setGainDecibels(apvts::gainDefaultValue);
	mOutputGainPtr->setGainDecibels(apvts::gainDefaultValue);

	for (const auto& parameterId : apvts::ids)
	{
		mAudioProcessorValueTreeStatePtr->addParameterListener(parameterId, this);
	}
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	for (const auto& parameterId : apvts::ids)
	{
		std::vector<std::string> parts;
		std::stringstream ss(parameterId);
		std::string item;

		while (std::getline(ss, item, '_')) {
			parts.push_back(item);
		}

		switch (parts.size())
		{
		case 2:
			if (parts[1] == apvts::onId)
			{
				layout.add(std::make_unique<juce::AudioParameterBool>(
					juce::ParameterID{ parameterId, apvts::version },
					parameterId,
					true
					));
			}
			else if (parts[1] == apvts::gainId)
			{
				layout.add(std::make_unique<juce::AudioParameterFloat>(
					juce::ParameterID{ parameterId, apvts::version },
					parameterId,
					apvts::gainNormalizableRange,
					apvts::gainDefaultValue
					));
			}
			else if (parts[1] == apvts::biasId)
			{
				layout.add(std::make_unique<juce::AudioParameterFloat>(
					juce::ParameterID{ parameterId, apvts::version },
					parameterId,
					apvts::biasNormalizableRange,
					apvts::biasDefaultValue
					));
			}
			else
			{
				assert(false);
			}

			break;
		case 3:
			if (parts[2] == apvts::onId)
			{
				layout.add(std::make_unique<juce::AudioParameterBool>(
					juce::ParameterID{ parameterId, apvts::version },
					parameterId,
					true
					));
			}
			else if (parts[1] == apvts::compressionId)
			{
				if (parts[2] == apvts::thresholdId)
				{
					layout.add(std::make_unique<juce::AudioParameterFloat>(
						juce::ParameterID{ parameterId, apvts::version },
						parameterId,
						apvts::thresholdNormalizableRange,
						apvts::thresholdDefaultValue
						));
				}
				else if (parts[2] == apvts::attackId)
				{
					layout.add(std::make_unique<juce::AudioParameterFloat>(
						juce::ParameterID{ parameterId, apvts::version },
						parameterId,
						apvts::attackNormalizableRange,
						apvts::attackDefaultValue
						));
				}
				else if (parts[2] == apvts::ratioId)
				{
					layout.add(std::make_unique<juce::AudioParameterFloat>(
						juce::ParameterID{ parameterId, apvts::version },
						parameterId,
						apvts::ratioNormalizableRange,
						apvts::ratioDefaultValue
						));
				}
				else if (parts[2] == apvts::releaseId)
				{
					layout.add(std::make_unique<juce::AudioParameterFloat>(
						juce::ParameterID{ parameterId, apvts::version },
						parameterId,
						apvts::releaseNormalizableRange,
						apvts::releaseDefaultValue
						));
				}
				else
				{
					assert(false);
				}
			}
			else if (parts[1] == apvts::highPassEqualizationId ||
				parts[1] == apvts::midPeakEqualizationId ||
				parts[1] == apvts::highShelfEqualizationId)
			{
				if (parts[2] == apvts::qualityId)
				{
					layout.add(std::make_unique<juce::AudioParameterFloat>(
						juce::ParameterID{ parameterId, apvts::version },
						parameterId,
						apvts::qualityNormalizableRange,
						apvts::qualityDefaultValue
						));
				}
				else if (parts[2] == apvts::frequencyId)
				{
					layout.add(std::make_unique<juce::AudioParameterFloat>(
						juce::ParameterID{ parameterId, apvts::version },
						parameterId,
						apvts::frequencyNormalizableRange,
						apvts::equalizationTypeIdToDefaultFrequencyMap.at(parts[1])
						));
				}
				else if (parts[2] == apvts::gainId)
				{
					layout.add(std::make_unique<juce::AudioParameterFloat>(
						juce::ParameterID{ parameterId, apvts::version },
						parameterId,
						apvts::eqGainNormalizableRange,
						apvts::eqGainDefaultValue
						));
				}
				else
				{
					assert(false);
				}
			}
			else
			{
				assert(false);
			}

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

	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

	mInputGainPtr->prepare(spec);

	mOverdriveTanhWaveShaperPtr->prepare(spec);
	mOverdriveTanhWaveShaperPtr->functionToUse = [](float x) {
		return std::tanh(x);
	};

	mOverdriveSoftClipWaveShaperPtr->prepare(spec);
	mOverdriveSoftClipWaveShaperPtr->functionToUse = [](float x) {
		return x / (std::abs(x) + 1.0f);
	};

	mOverdriveBiasPtr->prepare(spec);

	mOverdriveGainPtr->prepare(spec);

	mAmpImpulseResponseConvolutionPtr->prepare(spec);
	mAmpImpulseResponseConvolutionPtr->loadImpulseResponse(
		BinaryData::VTM60_low_pregain70_low50_mid50_high50_presence50_wav,
		BinaryData::VTM60_low_pregain70_low50_mid50_high50_presence50_wavSize,
		juce::dsp::Convolution::Stereo::yes,
		juce::dsp::Convolution::Trim::no,
		BinaryData::VTM60_low_pregain70_low50_mid50_high50_presence50_wavSize,
		juce::dsp::Convolution::Normalise::yes);

	mAmpCompressorPtr->prepare(spec);
	mAmpGainPtr->prepare(spec);

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

	for (const auto& parameterId : apvts::ids)
	{
		auto sampleRate = getSampleRate();
		auto newValue = mAudioProcessorValueTreeStatePtr->getParameterAsValue(parameterId).getValue();

		parameterChanged(parameterId, newValue);
	}
}

void PluginAudioProcessor::reset()
{
	mInputGainPtr->reset();
	mOverdriveTanhWaveShaperPtr->reset();
	mOverdriveSoftClipWaveShaperPtr->reset();
	mOverdriveBiasPtr->reset();
	mOverdriveGainPtr->reset();
	mAmpImpulseResponseConvolutionPtr->reset();
	mAmpCompressorPtr->reset();
	mAmpHighPassFilterPtr->reset();
	mAmpMidPeakFilterPtr->reset();
	mAmpHighShelfFilterPtr->reset();
	mAmpGainPtr->reset();
	mCabinetImpulseResponseConvolutionPtr->reset();
	mOutputGainPtr->reset();
}

void PluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
		buffer.clear(i, 0, buffer.getNumSamples());
	}

	auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
	auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);

	mInputGainPtr->process(context);

	if (mOverdriveTanhWaveShaperOn)
	{
		mOverdriveTanhWaveShaperPtr->process(context);
	}

	if (mOverdriveSoftClipWaveShaperOn)
	{
		mOverdriveSoftClipWaveShaperPtr->process(context);
	}

	mOverdriveBiasPtr->process(context);
	mOverdriveGainPtr->process(context);

	if (mAmpImpulseResponseConvolutionOn)
	{
		mAmpImpulseResponseConvolutionPtr->process(context);
	}

	mAmpCompressorPtr->process(context);

	if (mAmpLowShelfFilterOn)
	{
		mAmpHighPassFilterPtr->process(context);
	}

	if (mAmpMidPeakFilterOn)
	{
		mAmpMidPeakFilterPtr->process(context);
	}

	if (mAmpHighShelfFilterOn)
	{
		mAmpHighShelfFilterPtr->process(context);
	}

	mAmpGainPtr->process(context);

	if (mCabImpulseResponseConvolutionOn)
	{
		mCabinetImpulseResponseConvolutionPtr->process(context);
	}

	mOutputGainPtr->process(context);
}

void PluginAudioProcessor::parameterChanged(const juce::String& parameterIdJuceString, float newValue)
{
	std::string parameterId = parameterIdJuceString.toStdString();
	auto sampleRate = getSampleRate();

	std::vector<std::string> parts;
	std::stringstream ss(parameterId);
	std::string item;

	while (std::getline(ss, item, '_')) {
		parts.push_back(item);
	}

	switch (parts.size())
	{
	case 2:
		if (parts[1] == apvts::onId)
		{
			if (parameterId == apvts::ampImpulseResponseConvolutionOnId)
			{
				mAmpImpulseResponseConvolutionOn = newValue;
			}
			else if (parameterId == apvts::cabImpulseResponseConvolutionOnId)
			{
				mCabImpulseResponseConvolutionOn = newValue;
			}
		}
		else if (parts[1] == apvts::gainId)
		{
			if (parts[0] == apvts::inputId)
			{
				mInputGainPtr->setGainDecibels(newValue);
			}
			else if (parts[0] == apvts::ampId)
			{
				mAmpGainPtr->setGainDecibels(newValue);
			}
			else if (parts[0] == apvts::overdriveId)
			{
				mOverdriveGainPtr->setGainDecibels(newValue);
			}
			else if (parts[0] == apvts::outputId)
			{
				mOutputGainPtr->setGainDecibels(newValue);
			}
		}
		else if (parts[1] == apvts::biasId)
		{
			mOverdriveBiasPtr->setBias(newValue);
		}
		else
		{
			assert(false);
		}

		break;
	case 3:
		if (parts[2] == apvts::onId)
		{
			if (parameterId == apvts::overdriveTanhShaperOnId)
			{
				mOverdriveTanhWaveShaperOn = newValue;
			}
			else if (parameterId == apvts::overdriveClipShaperOnId)
			{
				mOverdriveSoftClipWaveShaperOn = newValue;
			}
			else if (parameterId == apvts::ampHighPassOnId)
			{
				mAmpLowShelfFilterOn = newValue;
			}
			else if (parameterId == apvts::ampMidPeakOnId)
			{
				mAmpMidPeakFilterOn = newValue;
			}
			else if (parameterId == apvts::ampHighShelfOnId)
			{
				mAmpHighShelfFilterOn = newValue;
			}
			else
			{
				assert(false);
			}
		}
		else if (parts[1] == apvts::compressionId)
		{
			if (parts[2] == apvts::thresholdId)
			{
				mAmpCompressorPtr->setThreshold(newValue);
			}
			else if (parts[2] == apvts::attackId)
			{
				mAmpCompressorPtr->setAttack(newValue);
			}
			else if (parts[2] == apvts::ratioId)
			{
				mAmpCompressorPtr->setRatio(newValue);
			}
			else if (parts[2] == apvts::releaseId)
			{
				mAmpCompressorPtr->setRelease(newValue);
			}
			else {
				assert(false);
			}
		}
		else if (parts[1] == apvts::highPassEqualizationId)
		{
			const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampHighPassFrequencyId).getValue();
			const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampHighPassQId).getValue();

			if (parts[2] == apvts::frequencyId)
			{
				*mAmpHighPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
					sampleRate,
					std::max(newValue, apvts::frequencyMinimumValue),
					std::max(quality, apvts::qualityMinimumValue));
			}
			else if (parts[2] == apvts::qualityId)
			{
				*mAmpHighPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
					sampleRate,
					std::max(frequency, apvts::frequencyMinimumValue),
					std::max(newValue, apvts::qualityMinimumValue));
			}
			else
			{
				assert(false);
			}
		}
		else if (parts[1] == apvts::midPeakEqualizationId)
		{
			const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampMidPeakFrequencyId).getValue();
			const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampMidPeakGainId).getValue();
			const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampMidPeakQId).getValue();

			if (parts[2] == apvts::frequencyId)
			{
				*mAmpMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
					sampleRate,
					std::max(newValue, apvts::frequencyMinimumValue),
					std::max(quality, apvts::qualityMinimumValue),
					juce::Decibels::decibelsToGain(gain));
			}
			else if (parts[2] == apvts::qualityId)
			{
				*mAmpMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
					sampleRate,
					std::max(frequency, apvts::frequencyMinimumValue),
					std::max(newValue, apvts::qualityMinimumValue),
					juce::Decibels::decibelsToGain(gain));
			}
			else if (parts[2] == apvts::gainId)
			{
				*mAmpMidPeakFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
					sampleRate,
					std::max(frequency, apvts::frequencyMinimumValue),
					std::max(quality, apvts::qualityMinimumValue),
					juce::Decibels::decibelsToGain(newValue));
			}
			else
			{
				assert(false);
			}
		}
		else if (parts[1] == apvts::highShelfEqualizationId)
		{
			const float frequency = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampHighShelfFrequencyId).getValue();
			const float gain = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampHighShelfGainId).getValue();
			const float quality = mAudioProcessorValueTreeStatePtr->getParameterAsValue(apvts::ampHighShelfQId).getValue();

			if (parts[2] == apvts::frequencyId)
			{
				*mAmpHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
					sampleRate,
					std::max(newValue, apvts::frequencyMinimumValue),
					std::max(quality, apvts::qualityMinimumValue),
					juce::Decibels::decibelsToGain(gain));
			}
			else if (parts[2] == apvts::qualityId)
			{
				*mAmpHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
					sampleRate,
					std::max(frequency, apvts::frequencyMinimumValue),
					std::max(newValue, apvts::qualityMinimumValue),
					juce::Decibels::decibelsToGain(gain));
			}
			else if (parts[2] == apvts::gainId)
			{
				*mAmpHighShelfFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
					sampleRate,
					std::max(frequency, apvts::frequencyMinimumValue),
					std::max(quality, apvts::qualityMinimumValue),
					juce::Decibels::decibelsToGain(newValue));
			}
			else
			{
				assert(false);
			}
		}
		else
		{
			assert(false);
		}

		break;
	default:
		assert(false);
		break;
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
