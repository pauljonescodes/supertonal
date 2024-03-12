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

	mInputGainPtr(std::make_unique<juce::dsp::Gain<float>>()),

	mNoiseGate(std::make_unique<juce::dsp::NoiseGate<float>>()),
	mPreCompressorPtr(std::make_unique<juce::dsp::Compressor<float>>()),
	mPreCompressorGainPtr(std::make_unique<juce::dsp::Gain<float>>()),

	mPreCompressorDryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),

	mTubeScreamerPtr(std::make_unique<TubeScreamer>()),
	mMouseDrivePtr(std::make_unique<MouseDrive>()),
	mGraphicEqualiser(std::make_unique<GraphicEqualiser>()),

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
	mAmplifierEqualiser(std::make_unique<AmplifierEqualiser>()),

	mPostCompressorPtr(std::make_unique<juce::dsp::Compressor<float>>()),
	mPostCompressorGainPtr(std::make_unique<juce::dsp::Gain<float>>()),
	mPostCompressorDryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),

	mDelayLineLeftPtr(std::make_unique<juce::dsp::DelayLine<float>>(apvts::delayTimeMsMaximumValue* (apvts::sampleRateAssumption / 1000))),
	mDelayLineRightPtr(std::make_unique<juce::dsp::DelayLine<float>>(apvts::delayTimeMsMaximumValue* (apvts::sampleRateAssumption / 1000))),
	mDelayLineDryWetMixerPtr(std::make_unique<juce::dsp::DryWetMixer<float>>()),
	mDelayHighPassFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>(
		juce::dsp::IIR::Coefficients<float>::makeHighPass(apvts::sampleRateAssumption, InstrumentEqualiser::sHighPassFrequencyNormalisableRange.start)
		)),
	mDelayLowPassFilterPtr(std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>(
		juce::dsp::IIR::Coefficients<float>::makeLowPass(apvts::sampleRateAssumption, InstrumentEqualiser::sLowPassFrequencyNormalisableRange.start)
		)),

	mChorusPtr(std::make_unique<juce::dsp::Chorus<float>>()),
	mPhaserPtr(std::make_unique<juce::dsp::Phaser<float>>()),

	mConvolutionMessageQueuePtr(std::make_unique<juce::dsp::ConvolutionMessageQueue>()),
	mCabinetImpulseResponseConvolutionPtr(std::make_unique<juce::dsp::Convolution>(juce::dsp::Convolution::NonUniform{ 128 }, * mConvolutionMessageQueuePtr.get())),
	
	mInstrumentCompressor(std::make_unique<Compressor>()),
	mInstrumentEqualiser(std::make_unique<InstrumentEqualiser>()),

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
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_ON:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_ON:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_ON:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_ON:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_ON:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_ON:
		case apvts::ParameterEnum::BYPASS_ON:
		case apvts::ParameterEnum::TUBE_SCREAMER_ON:
		case apvts::ParameterEnum::MOUSE_DRIVE_ON:
		case apvts::ParameterEnum::STAGE2_ON:
		case apvts::ParameterEnum::STAGE3_ON:
		case apvts::ParameterEnum::STAGE4_ON:
		case apvts::ParameterEnum::REVERB_ON:
		case apvts::ParameterEnum::PRE_COMPRESSOR_IS_ON:
		case apvts::ParameterEnum::POST_COMPRESSOR_IS_ON:
		case apvts::ParameterEnum::DELAY_ON:
		case apvts::ParameterEnum::PHASER_ON:
		case apvts::ParameterEnum::CHORUS_ON:
		case apvts::ParameterEnum::PRE_EQUALISER_ON:
		case apvts::ParameterEnum::DELAY_IS_SYNCED:
		case apvts::ParameterEnum::PRE_COMPRESSOR_AUTO_MAKE_UP_ON:
		case apvts::ParameterEnum::POST_COMPRESSOR_AUTO_MAKE_UP_ON:
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_IS_PRE_EQ_ON:
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_IS_ON:
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_AUTO_GAIN_ON:
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_AUTO_ATTACK_ON:
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_AUTO_RELEASE_ON:
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_LOOKAHEAD_ON:
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
		case apvts::ParameterEnum::REVERB_MIX:
		case apvts::ParameterEnum::REVERB_SIZE:
		case apvts::ParameterEnum::REVERB_DAMPING:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::zeroToOneLinearNormalisableRange,
				apvts::defaultValueQuarter
				));
			break;
		case apvts::ParameterEnum::PRE_EQUALISER_100_GAIN:
		case apvts::ParameterEnum::PRE_EQUALISER_200_GAIN:
		case apvts::ParameterEnum::PRE_EQUALISER_400_GAIN:
		case apvts::ParameterEnum::PRE_EQUALISER_800_GAIN:
		case apvts::ParameterEnum::PRE_EQUALISER_1600_GAIN:
		case apvts::ParameterEnum::PRE_EQUALISER_3200_GAIN:
		case apvts::ParameterEnum::PRE_EQUALISER_6400_GAIN:
		case apvts::ParameterEnum::PRE_EQUALISER_LEVEL_GAIN:
		{
			juce::NormalisableRange<float> normalisableRange = GraphicEqualiser::sDecibelGainNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				apvts::defaultValueOff
				));
		}
		break;
		case apvts::ParameterEnum::AMP_RESONANCE_DB:
		case apvts::ParameterEnum::AMP_BASS_DB:
		case apvts::ParameterEnum::AMP_MIDDLE_DB:
		case apvts::ParameterEnum::AMP_TREBLE_DB:
		case apvts::ParameterEnum::AMP_PRESENCE_DB:
		{
			juce::NormalisableRange<float> normalisableRange = AmplifierEqualiser::sDecibelsNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				apvts::defaultValueOff
				));
		}
		break;
		case apvts::ParameterEnum::STAGE1_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE1_OUTPUT_GAIN:
		case apvts::ParameterEnum::STAGE2_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE2_OUTPUT_GAIN:
		case apvts::ParameterEnum::STAGE3_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE3_OUTPUT_GAIN:
		case apvts::ParameterEnum::STAGE4_INPUT_GAIN:
		case apvts::ParameterEnum::STAGE4_OUTPUT_GAIN:
		case apvts::ParameterEnum::INPUT_GAIN:
		case apvts::ParameterEnum::POST_COMPRESSOR_GAIN:
		case apvts::ParameterEnum::OUTPUT_GAIN:
		case apvts::ParameterEnum::PRE_COMPRESSOR_GAIN:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNormalisableRange,
				apvts::gainDeciblesDefaultValue
				));
			break;
		case apvts::ParameterEnum::CABINET_OUTPUT_GAIN:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNormalisableRange,
				10.0f
				));
			break;
		case apvts::ParameterEnum::PRE_COMPRESSOR_THRESHOLD:
		case apvts::ParameterEnum::POST_COMPRESSOR_THRESHOLD:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNegativeNormalisableRange,
				apvts::gainDeciblesDefaultValue
				));
			break;
		case apvts::ParameterEnum::MOUSE_DRIVE_VOLUME:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNegativeNormalisableRange,
				MouseDrive::sVolumeDefaultValue
				));
			break;
		case apvts::ParameterEnum::TUBE_SCREAMER_LEVEL:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::gainDecibelsNegativeNormalisableRange,
				TubeScreamer::sLevelDefaultValue
				));
			break;
		case apvts::ParameterEnum::CHORUS_MIX:
		case apvts::ParameterEnum::PHASER_MIX:
		case apvts::ParameterEnum::STAGE1_DRY_WET_MIX:
		case apvts::ParameterEnum::STAGE2_DRY_WET_MIX:
		case apvts::ParameterEnum::STAGE3_DRY_WET_MIX:
		case apvts::ParameterEnum::STAGE4_DRY_WET_MIX:
		case apvts::ParameterEnum::PRE_COMPRESSOR_DRY_WET_MIX:
		case apvts::ParameterEnum::POST_COMPRESSOR_DRY_WET_MIX:
		case apvts::ParameterEnum::TUBE_SCREAMER_TONE:
		case apvts::ParameterEnum::MOUSE_DRIVE_DISTORTION:
		case apvts::ParameterEnum::REVERB_WIDTH:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::zeroToOneLinearNormalisableRange,
				apvts::defaultValueOn
				));
			break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_GAIN:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_GAIN:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_GAIN:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_GAIN:
		{
			juce::NormalisableRange<float> normalisableRange = InstrumentEqualiser::sDecibelsNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				apvts::defaultValueOff
				));
		}
		break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_QUALITY:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_QUALITY:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_QUALITY:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_QUALITY:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_QUALITY:
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_QUALITY:
		{
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::qualityNormalisableRange,
				apvts::qualityDefaultValue
				));
		}
		break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_FREQUENCY:
		case apvts::ParameterEnum::DELAY_LOW_PASS_FREQUENCY:
		{
			juce::NormalisableRange<float> normalisableRange = InstrumentEqualiser::sLowPassFrequencyNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				InstrumentEqualiser::sLowPassFrequencyDefaultValue
				));
		}
		break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_FREQUENCY:
		{
			juce::NormalisableRange<float> normalisableRange = InstrumentEqualiser::sLowPeakFrequencyNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				InstrumentEqualiser::sLowPeakFrequencyDefaultValue
				));
		}
		break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_FREQUENCY:
		{
			juce::NormalisableRange<float> normalisableRange = InstrumentEqualiser::sLowMidPeakFrequencyNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				InstrumentEqualiser::sLowMidPeakFrequencyDefaultValue
				));
		}
		break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_FREQUENCY:
		{
			juce::NormalisableRange<float> normalisableRange = InstrumentEqualiser::sHighMidPeakFrequencyNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				InstrumentEqualiser::sHighMidPeakFrequencyDefaultValue
				));
		}
		break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_FREQUENCY:
		{
			juce::NormalisableRange<float> normalisableRange = InstrumentEqualiser::sHighPeakFrequencyNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				InstrumentEqualiser::sHighPeakFrequencyDefaultValue
				));
		}
		break;
		case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_FREQUENCY:
		case apvts::ParameterEnum::DELAY_HIGH_PASS_FREQUENCY:
		{
			juce::NormalisableRange<float> normalisableRange = InstrumentEqualiser::sHighPassFrequencyNormalisableRange;
			normalisableRange.interval = apvts::defaultIntervalValue;
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				normalisableRange,
				InstrumentEqualiser::sHighPassFrequencyDefaultValue
				));
		}
		break;
		case apvts::ParameterEnum::STAGE1_ON:
		case apvts::ParameterEnum::LIMITER_ON:
		case apvts::ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON:
			layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ parameterId, apvts::version }, parameterId, true ));
			break;
		case apvts::ParameterEnum::TUBE_SCREAMER_DRIVE:
		case apvts::ParameterEnum::DELAY_DRY_WET:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::zeroToOneLinearNormalisableRange,
				apvts::defaultValueHalf
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
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::ratioNormalizableRange,
				apvts::ratioDefaultValue
				));
			break;
		case apvts::ParameterEnum::NOISE_GATE_RATIO:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::ratioNormalizableRange,
				100.0f
				));
			break;
		case apvts::ParameterEnum::MOUSE_DRIVE_FILTER:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::frequencyNormalisableRange,
				apvts::lowPassFrequencyDefaultValue
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
		case apvts::ParameterEnum::DELAY_RIGHT_MS:
		case apvts::ParameterEnum::DELAY_LEFT_MS:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::delayTimeMsNormalisableRange,
				apvts::delayTimeMsDefaultValue
				));
			break;
		case apvts::ParameterEnum::DELAY_LEFT_PER_BEAT:
		case apvts::ParameterEnum::DELAY_RIGHT_PER_BEAT:
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				apvts::fractionalTimeNormalizableRange,
				2.0f
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
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_INPUT_GAIN:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::inputStart,
					apvts::Ctagdrc::inputEnd,
					apvts::Ctagdrc::inputInterval), 0.0f,
				String(),
				AudioProcessorParameter::genericParameter,
				[](float value, float)
				{
					return String(value, 1) + " dB";
				}));
			break;
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_MAKEUP_GAIN:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::makeupStart,
					apvts::Ctagdrc::makeupEnd,
					apvts::Ctagdrc::makeupInterval), 
				apvts::Ctagdrc::makeupDefault,
				String(),
				AudioProcessorParameter::genericParameter,
				[](float value, float)
				{
					return String(value, 1) + " dB ";
				}));
			break;
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_THRESHOLD:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::thresholdStart,
					apvts::Ctagdrc::thresholdEnd,
					apvts::Ctagdrc::thresholdInterval), 
				apvts::Ctagdrc::thresholdDefault,
				String(), AudioProcessorParameter::genericParameter,
				[](float value, float maxStrLen)
				{
					return String(value, 1) + " dB";
				}));
			break;
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_RATIO:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::ratioStart,
					apvts::Ctagdrc::ratioEnd,
					apvts::Ctagdrc::ratioInterval, 
					0.5f), 
				apvts::Ctagdrc::ratioDefault,
				String(), AudioProcessorParameter::genericParameter,
				[](float value, float)
				{
					if (value > 23.9f)return String("Infinity") + ":1";
			return String(value, 1) + ":1";
				}));
			break;
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_KNEE:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::kneeStart,
					apvts::Ctagdrc::kneeEnd,
					apvts::Ctagdrc::kneeInterval),
				6.0f, String(), AudioProcessorParameter::genericParameter,
				[](float value, float)
				{
					return String(value, 1) + " dB";
				}));
			break;
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_ATTACK:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::attackStart,
					apvts::Ctagdrc::attackEnd,
					apvts::Ctagdrc::attackInterval, 0.5f), 
				apvts::Ctagdrc::attackDefault,
				"ms",
				AudioProcessorParameter::genericParameter,
				[](float value, float)
				{
					if (value == 100.0f) return String(value, 0) + " ms";
			return String(value, 2) + " ms";
				}));
			break;
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_RELEASE:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::releaseStart,
					apvts::Ctagdrc::releaseEnd,
					apvts::Ctagdrc::releaseInterval, 0.35f),
				apvts::Ctagdrc::releaseDefault,
				String(),
				AudioProcessorParameter::genericParameter,
				[](float value, float)
				{
					if (value <= 100) return String(value, 2) + " ms";
			if (value >= 1000)
				return String(value * 0.001f, 2) + " s";
			return String(value, 1) + " ms";
				}));
			break;
		case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_MIX:
			layout.add(std::make_unique<AudioParameterFloat>(
				juce::ParameterID{ parameterId, apvts::version },
				PluginUtils::toTitleCase(parameterId),
				NormalisableRange<float>(
					apvts::Ctagdrc::mixStart,
					apvts::Ctagdrc::mixEnd,
					apvts::Ctagdrc::mixInterval),
				1.0f, "%", AudioProcessorParameter::genericParameter,
				[](float value, float)
				{
					return String(value * 100.0f, 1) + " %";
				}));
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

	mInputGainPtr->prepare(spec);

	mNoiseGate->prepare(spec);
	mPreCompressorPtr->prepare(spec);
	mPreCompressorGainPtr->prepare(spec);
	mPreCompressorDryWetMixerPtr->prepare(spec);

	mGraphicEqualiser->prepare(spec);

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
	mAmplifierEqualiser->prepare(spec);

	mPostCompressorPtr->prepare(spec);
	mPostCompressorGainPtr->prepare(spec);
	mPostCompressorDryWetMixerPtr->prepare(spec);

	const auto maximumDelayInSamples = apvts::delayTimeMsMaximumValue * (spec.sampleRate / 1000);
	mDelayLineLeftPtr->setMaximumDelayInSamples(maximumDelayInSamples);
	mDelayLineLeftPtr->prepare(spec);
	mDelayLineRightPtr->setMaximumDelayInSamples(maximumDelayInSamples);
	mDelayLineRightPtr->prepare(spec);
	mDelayLineDryWetMixerPtr->prepare(spec);

	mDelayLowPassFilterPtr->prepare(spec);
	mDelayHighPassFilterPtr->prepare(spec);

	mChorusPtr->prepare(spec);
	mPhaserPtr->prepare(spec);

	mCabinetImpulseResponseConvolutionPtr->prepare(spec);
	loadImpulseResponseFromState();

	mInstrumentEqualiser->prepare(spec);
	mInstrumentCompressor->prepare(spec);

	if (*mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::instrumentCompressorIsLookaheadOn) > 0.5f)
	{
		setLatencySamples(static_cast<int>(0.005 * sampleRate));
	}
	else
	{
		setLatencySamples(0);
	}
		
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
	mInputGainPtr->reset();

	mNoiseGate->reset();
	mPreCompressorPtr->reset();
	mPreCompressorGainPtr->reset();

	mPreCompressorDryWetMixerPtr->reset();

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
	mAmplifierEqualiser->reset();

	mPostCompressorPtr->reset();
	mPostCompressorGainPtr->reset();
	mPostCompressorDryWetMixerPtr->reset();

	mDelayLineLeftPtr->reset();
	mDelayLineRightPtr->reset();
	mDelayLineDryWetMixerPtr->reset();
	mDelayLowPassFilterPtr->reset();
	mDelayHighPassFilterPtr->reset();

	mChorusPtr->reset();
	mPhaserPtr->reset();

	mCabinetImpulseResponseConvolutionPtr->reset();
	mInstrumentEqualiser->reset();
	//mInstrumentCompressor->reset();

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

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
	{
		buffer.clear(i, 0, numSamples);
	}

	auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
	auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

	mNoiseGate->process(processContext);
	mInputGainPtr->process(processContext);

	if (mPreCompressorIsOn)
	{
		mPreCompressorDryWetMixerPtr->pushDrySamples(audioBlock);
		float preCompressorInputRms = mPreCompressorAutoMakeup ? calculateRMS(buffer, totalNumInputChannels, numSamples) : 0;
		mPreCompressorPtr->process(processContext);

		if (mPreCompressorAutoMakeup)
		{
			float preCompressorOutputRms = calculateRMS(buffer, totalNumInputChannels, numSamples);
			mPreCompressorGainSmoothedValue.setTargetValue(std::min(preCompressorInputRms / preCompressorOutputRms, 12.0f));

			for (int sample = 0; sample < numSamples; ++sample)
			{
				float preCompressorGainSmoothedNextValue = mPreCompressorGainSmoothedValue.getNextValue();

				for (int channel = 0; channel < totalNumInputChannels; ++channel)
				{
					auto* channelData = buffer.getWritePointer(channel);
					channelData[sample] *= preCompressorGainSmoothedNextValue;
				}
			}
		}

		mPreCompressorGainPtr->process(processContext);
		mPreCompressorDryWetMixerPtr->mixWetSamples(audioBlock);
	}

	if (mGraphicEqualiserIsOn)
	{
		mGraphicEqualiser->processBlock(buffer);
	}

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

	mAmplifierEqualiser->processBlock(buffer);
	mBiasPtr->process(processContext);

	if (mPostCompressorIsOn)
	{
		mPostCompressorDryWetMixerPtr->pushDrySamples(audioBlock);
		float postCompressorInputRms = mPostCompressorAutoMakeup ? calculateRMS(buffer, totalNumInputChannels, numSamples) : 0;
		mPostCompressorPtr->process(processContext);

		if (mPostCompressorAutoMakeup)
		{
			float postCompressorOutputRms = calculateRMS(buffer, totalNumInputChannels, numSamples);
			mPostCompressorGainSmoothedValue.setTargetValue(std::min(postCompressorInputRms / postCompressorOutputRms, 12.0f));

			for (int sample = 0; sample < numSamples; ++sample)
			{
				float preCompressorGainSmoothedNextValue = mPostCompressorGainSmoothedValue.getNextValue();

				for (int channel = 0; channel < totalNumInputChannels; ++channel)
				{
					auto* channelData = buffer.getWritePointer(channel);
					channelData[sample] *= preCompressorGainSmoothedNextValue;
				}
			}
		}

		mPostCompressorGainPtr->process(processContext);
		mPostCompressorDryWetMixerPtr->mixWetSamples(audioBlock);
	}

	if (mDelayFeedback > 0.0f && mDelayOn)
	{
		mDelayLineDryWetMixerPtr->pushDrySamples(audioBlock);

		auto* leftChannelData = audioBlock.getChannelPointer(0);
		if (leftChannelData)
		{
			float fractionOfWholeBeatLeft = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::delayLeftPerBeatId);
			const auto nextDelayLeftSamples = mDelayBpmSynced ?
				PluginUtils::calculateSamplesForBpmFractionAndRate(smoothedBeatsPerMinute, fractionOfWholeBeatLeft, samplesPerSecond) :
				PluginUtils::calculateSamplesForMilliseconds(samplesPerSecond, mDelayLeftMilliseconds);

			if (mDelayLineLeftPtr->getDelay() != nextDelayLeftSamples)
			{
				mDelayLineLeftPtr->setDelay(nextDelayLeftSamples);
			}


			for (int i = 0; i < numSamples; ++i)
			{
				const float delayedSample = mDelayLineLeftPtr->popSample(0, mDelayLineLeftPtr->getDelay());

				if (i < audioBlock.getNumSamples())
				{
					mDelayLineLeftPtr->pushSample(0, leftChannelData[i] + mDelayFeedback * delayedSample);
					leftChannelData[i] += delayedSample;
				}
			}
		}

		auto* rightChannelData = audioBlock.getChannelPointer(1);
		if (rightChannelData)
		{
			float fractionOfWholeBeatRight = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::delayRightPerBeatId);
			const auto nextDelayRightSamples = mDelayBpmSynced ?
				PluginUtils::calculateSamplesForBpmFractionAndRate(smoothedBeatsPerMinute, fractionOfWholeBeatRight, samplesPerSecond) :
				PluginUtils::calculateSamplesForMilliseconds(samplesPerSecond, mDelayRightMilliseconds);

			if (mDelayLineLeftPtr->getDelay() != nextDelayRightSamples)
			{
				mDelayLineLeftPtr->setDelay(nextDelayRightSamples);
			}

			for (int i = 0; i < numSamples; ++i)
			{
				const float delayedSample = mDelayLineRightPtr->popSample(0, mDelayLineRightPtr->getDelay());

				if (i < audioBlock.getNumSamples())
				{
					mDelayLineRightPtr->pushSample(0, rightChannelData[i] + mDelayFeedback * delayedSample);
					rightChannelData[i] += delayedSample;
				}
			}
		}

		juce::dsp::ProcessContextReplacing<float> wetContext(audioBlock);

		mDelayLowPassFilterPtr->process(wetContext);
		mDelayHighPassFilterPtr->process(wetContext);

		mDelayLineDryWetMixerPtr->mixWetSamples(audioBlock);
	}

	if (mChorusOn)
	{
		float chorusFractionOfBeat = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::chorusFractionOfBeatId);
		float chorusSmoothedRate = apvts::clampedValueForFractionOfBeat(smoothedBeatsPerMinute, chorusFractionOfBeat);
		mChorusPtr->setRate(chorusSmoothedRate);

		float chorusDelayFractionOfBeat = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::chorusCenterDelayFractionOfBeatId);
		float chorusSmoothedDelay = apvts::clampedValueForFractionOfBeat(smoothedBeatsPerMinute, chorusDelayFractionOfBeat);
		mChorusPtr->setCentreDelay(chorusSmoothedDelay);

		mChorusPtr->process(processContext);
	}

	if (mPhaserOn)
	{
		float phaserFractionOfBeat = *mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::phaserRateFractionOfBeatId);
		float phaserSmoothedRate = apvts::clampedValueForFractionOfBeat(smoothedBeatsPerMinute, phaserFractionOfBeat);
		mPhaserPtr->setRate(phaserSmoothedRate);

		mPhaserPtr->process(processContext);
	}

	if (mReverbOn)
	{
		mReverb->process(processContext);
	}

	if (mCabImpulseResponseConvolutionIsOn)
	{
		mCabinetImpulseResponseConvolutionPtr->process(processContext);
		mCabinetGainPtr->process(processContext);
	}

	if (mInstrumentCompressorIsPreEqualiser)
	{
		mInstrumentCompressor->process(buffer);
	}

	mInstrumentEqualiser->processBlock(buffer);

	if (!mInstrumentCompressorIsPreEqualiser)
	{
		mInstrumentCompressor->process(buffer);
	}

	if (mLimiterOn)
	{
		mLimiter->process(processContext);
	}

	mOutputGainPtr->process(processContext);

#ifdef JUCE_DEBUG
	checkForInvalidSamples(audioBlock);
#endif
}

void PluginAudioProcessor::checkForInvalidSamples(const juce::dsp::AudioBlock<float>& blockToCheck)
{
	auto numChans = blockToCheck.getNumChannels();
	auto numSamps = blockToCheck.getNumSamples();

	for (auto c = 0; c < numChans; ++c)
	{
		for (auto s = 0; s < numSamps; ++s)
		{
			auto sample = blockToCheck.getSample(c, s);
			jassert(!std::isnan(sample));
			// Probably also this ones
			jassert(sample <= 1.0f);
			jassert(sample >= -1.0f);
		}
	}
}

void PluginAudioProcessor::parameterChanged(const juce::String& parameterIdJuceString, float newValue)
{
	auto sampleRate = getSampleRate();
	auto playhead = this->getPlayHead();
	double beatsPerMinute = 120; // Default fallback BPM

	if (playhead != nullptr) // Check if playhead is valid
	{
		juce::AudioPlayHead::CurrentPositionInfo positionInfo;

		if (playhead->getCurrentPosition(positionInfo)) // Check if position info is valid
		{
			beatsPerMinute = positionInfo.bpm; // Use actual BPM if available
		}
	}

	switch (apvts::parameterIdToEnumMap.at(parameterIdJuceString.toStdString()))
	{
	case apvts::ParameterEnum::INPUT_GAIN:
		mInputGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::REVERB_ON:
		mReverbOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::LIMITER_ON:
		mLimiterOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::BYPASS_ON:
		mBypassIsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::STAGE1_ON:
		mStage1IsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::STAGE2_ON:
		mStage2IsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::STAGE3_ON:
		mStage3IsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::STAGE4_ON:
		mStage4IsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_ON:
		mGraphicEqualiserIsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::PRE_COMPRESSOR_AUTO_MAKE_UP_ON:
		mPreCompressorAutoMakeup = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_AUTO_MAKE_UP_ON:
		mPostCompressorAutoMakeup = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_DRY_WET_MIX:
		mPostCompressorDryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_100_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 0);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_200_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 1);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_400_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 2);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_800_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 3);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_1600_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 4);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_3200_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 5);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_6400_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 6);
		break;
	case apvts::ParameterEnum::PRE_EQUALISER_LEVEL_GAIN:
		mGraphicEqualiser->setGainDecibelsAtIndex(newValue, 7);
		break;
	case apvts::ParameterEnum::AMP_RESONANCE_DB:
		mAmplifierEqualiser->setResonanceDecibels(newValue);
		break;
	case apvts::ParameterEnum::AMP_BASS_DB:
		mAmplifierEqualiser->setBassDecibels(newValue);
		break;
	case apvts::ParameterEnum::AMP_MIDDLE_DB:
		mAmplifierEqualiser->setMiddleDecibels(newValue);
		break;
	case apvts::ParameterEnum::AMP_TREBLE_DB:
		mAmplifierEqualiser->setTrebleDecibels(newValue);
		break;
	case apvts::ParameterEnum::AMP_PRESENCE_DB:
		mAmplifierEqualiser->setPresenceDecibels(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_IS_ON:
		mPostCompressorIsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::PRE_COMPRESSOR_IS_ON:
		mPreCompressorIsOn = static_cast<bool>(newValue);
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
	case apvts::ParameterEnum::STAGE1_DRY_WET_MIX:
		mStage1DryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::STAGE2_DRY_WET_MIX:
		mStage2DryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::STAGE3_DRY_WET_MIX:
		mStage3DryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::STAGE4_DRY_WET_MIX:
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
	case apvts::ParameterEnum::PRE_COMPRESSOR_GAIN:
		mPreCompressorGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::PRE_COMPRESSOR_DRY_WET_MIX:
		mPreCompressorDryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::POST_COMPRESSOR_GAIN:
		mPostCompressorGainPtr->setGainDecibels(newValue);
		break;
	case apvts::ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON:
		mCabImpulseResponseConvolutionIsOn = static_cast<bool>(newValue);
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
		mReverb->setParameters({ newValue, parameters.damping, parameters.wetLevel, parameters.dryLevel, parameters.width, 0.0f });
	}
	break;
	case apvts::ParameterEnum::REVERB_DAMPING:
	{
		const auto& parameters = mReverb->getParameters();
		mReverb->setParameters({ parameters.roomSize, newValue, parameters.wetLevel, parameters.dryLevel, parameters.width, 0.0f });
	}
	break;
	case apvts::ParameterEnum::REVERB_MIX:
	{
		const auto& parameters = mReverb->getParameters();
		mReverb->setParameters({ parameters.roomSize, parameters.damping, newValue, 1.0f - newValue, parameters.width, 0.0f });
	}
	break;
	case apvts::ParameterEnum::REVERB_WIDTH:
	{
		const auto& parameters = mReverb->getParameters();
		mReverb->setParameters({ parameters.roomSize, parameters.damping, parameters.wetLevel, parameters.dryLevel, newValue, 0.0f });
	}
	break;
	case apvts::ParameterEnum::DELAY_DRY_WET:
		mDelayLineDryWetMixerPtr->setWetMixProportion(newValue);
		break;
	case apvts::ParameterEnum::DELAY_FEEDBACK:
		mDelayFeedback = newValue;
		break;
	case apvts::ParameterEnum::DELAY_LOW_PASS_FREQUENCY:
		*mDelayLowPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, std::max(newValue, apvts::defaultEpsilon), 0.7);
		break;
	case apvts::ParameterEnum::DELAY_HIGH_PASS_FREQUENCY:
		*mDelayHighPassFilterPtr->state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, std::max(newValue, apvts::defaultEpsilon), 0.7);
		break;
	case apvts::ParameterEnum::DELAY_LEFT_MS:
		mDelayLineLeftPtr->setDelay(PluginUtils::calculateSamplesForMilliseconds(sampleRate, newValue));
		break;
	case apvts::ParameterEnum::DELAY_RIGHT_MS:
		mDelayLineRightPtr->setDelay(PluginUtils::calculateSamplesForMilliseconds(sampleRate, newValue));
		break;
	case apvts::ParameterEnum::DELAY_LEFT_PER_BEAT:
		mDelayLineLeftPtr->setDelay(PluginUtils::calculateSamplesForBpmFractionAndRate(beatsPerMinute, newValue, sampleRate));
		break;
	case apvts::ParameterEnum::DELAY_RIGHT_PER_BEAT:
		mDelayLineRightPtr->setDelay(PluginUtils::calculateSamplesForBpmFractionAndRate(beatsPerMinute, newValue, sampleRate));
		break;
	case apvts::ParameterEnum::DELAY_IS_SYNCED:
		mDelayBpmSynced = newValue;
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
	case apvts::ParameterEnum::MOUSE_DRIVE_FILTER:
		mMouseDrivePtr->setLowPassFrequency(newValue);
		break;
	case apvts::ParameterEnum::MOUSE_DRIVE_VOLUME:
		mMouseDrivePtr->setVolume(newValue);
		break;
	case apvts::ParameterEnum::MOUSE_DRIVE_ON:
		mMouseDriveIsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_ON:
		mTubeScreamerIsOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_DRIVE:
		mTubeScreamerPtr->setDrive(newValue);
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_LEVEL:
		mTubeScreamerPtr->setLevel(newValue);
		break;
	case apvts::ParameterEnum::TUBE_SCREAMER_TONE:
		mTubeScreamerPtr->setTone(newValue);
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
	case apvts::ParameterEnum::DELAY_ON:
		mDelayOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::PHASER_ON:
		mPhaserOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::CHORUS_ON:
		mChorusOn = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_ON:
		mInstrumentEqualiser->setOnAtIndex(static_cast<bool>(newValue), 0);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_FREQUENCY:
		mInstrumentEqualiser->setFrequencyAtIndex(newValue, 0);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_QUALITY:
		mInstrumentEqualiser->setQualityAtIndex(newValue, 0);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_ON:
		mInstrumentEqualiser->setOnAtIndex(static_cast<bool>(newValue), 1);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_FREQUENCY:
		mInstrumentEqualiser->setFrequencyAtIndex(newValue, 1);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_GAIN:
		mInstrumentEqualiser->setGainAtIndex(newValue, 1);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_QUALITY:
		mInstrumentEqualiser->setQualityAtIndex(newValue, 1);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_ON:
		mInstrumentEqualiser->setOnAtIndex(static_cast<bool>(newValue), 2);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_FREQUENCY:
		mInstrumentEqualiser->setFrequencyAtIndex(newValue, 2);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_GAIN:
		mInstrumentEqualiser->setGainAtIndex(newValue, 2);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_QUALITY:
		mInstrumentEqualiser->setQualityAtIndex(newValue, 2);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_ON:
		mInstrumentEqualiser->setOnAtIndex(static_cast<bool>(newValue), 3);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_FREQUENCY:
		mInstrumentEqualiser->setFrequencyAtIndex(newValue, 3);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_GAIN:
		mInstrumentEqualiser->setGainAtIndex(newValue, 3);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_QUALITY:
		mInstrumentEqualiser->setQualityAtIndex(newValue, 3);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_ON:
		mInstrumentEqualiser->setOnAtIndex(static_cast<bool>(newValue), 4);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_FREQUENCY:
		mInstrumentEqualiser->setFrequencyAtIndex(newValue, 4);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_GAIN:
		mInstrumentEqualiser->setGainAtIndex(newValue, 4);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_QUALITY:
		mInstrumentEqualiser->setQualityAtIndex(newValue, 4);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_ON:
		mInstrumentEqualiser->setOnAtIndex(static_cast<bool>(newValue), 5);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_FREQUENCY:
		mInstrumentEqualiser->setFrequencyAtIndex(newValue, 5);
		break;
	case apvts::ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_QUALITY:
		mInstrumentEqualiser->setQualityAtIndex(newValue, 5);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_IS_PRE_EQ_ON:
		mInstrumentCompressorIsPreEqualiser = static_cast<bool>(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_IS_ON:
		mInstrumentCompressor->setPower(!static_cast<bool>(newValue));
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_LOOKAHEAD_ON:
	{
		const bool newBool = static_cast<bool>(newValue);
		if (newBool)
		{
			setLatencySamples(static_cast<int>(0.005 * mInstrumentCompressor->getSampleRate()));
		}
		else 
		{ 
			setLatencySamples(0); 
		}

		mInstrumentCompressor->setLookahead(newBool);
	}
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_AUTO_GAIN_ON:
		mInstrumentCompressor->setAutoMakeup(static_cast<bool>(newValue));
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_AUTO_ATTACK_ON:
	{
		const bool newBool = static_cast<bool>(newValue);
		mInstrumentCompressor->setAutoAttack(newBool);

		if (!newBool)
		{
			mInstrumentCompressor->setAttack(*mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::instrumentCompressorAttack));
		}
	}
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_AUTO_RELEASE_ON:
	{
		const bool newBool = static_cast<bool>(newValue);
		mInstrumentCompressor->setAutoRelease(newBool);
		if (!newBool)
		{
			mInstrumentCompressor->setRelease(*mAudioProcessorValueTreeStatePtr->getRawParameterValue(apvts::instrumentCompressorRelease));
		}
	}
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_INPUT_GAIN:
		mInstrumentCompressor->setInput(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_MAKEUP_GAIN:
		mInstrumentCompressor->setMakeup(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_THRESHOLD:
		mInstrumentCompressor->setThreshold(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_RATIO:
		mInstrumentCompressor->setRatio(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_KNEE:
		mInstrumentCompressor->setKnee(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_ATTACK:
		mInstrumentCompressor->setAttack(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_RELEASE:
		mInstrumentCompressor->setRelease(newValue);
		break;
	case apvts::ParameterEnum::INSTRUMENT_COMPRESSOR_MIX:
		mInstrumentCompressor->setMix(newValue);
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
	juce::File* impulseResponseFile = new juce::File(impulseResponseFullPathName);

	if (impulseResponseFullPathName.length() > 0)
	{
		mCabinetImpulseResponseConvolutionPtr->loadImpulseResponse(
			*impulseResponseFile,
			juce::dsp::Convolution::Stereo::yes,
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

float PluginAudioProcessor::calculateRMS(juce::AudioBuffer<float>& buffer, int numChannels, int numSamples)
{
	float sum = 0.0f;

	for (int channel = 0; channel < numChannels; ++channel) {
		const float* channelData = buffer.getReadPointer(channel, 0);

		for (int i = 0; i < numSamples; ++i) {
			float sample = channelData[i];
			sum += sample * sample; // Square the sample and add it to the sum.
		}
	}

	float mean = sum / (numChannels * numSamples); // Calculate the mean of the squared sums.
	return std::sqrt(mean) + 0.0001; // Return the square root of the mean.
}