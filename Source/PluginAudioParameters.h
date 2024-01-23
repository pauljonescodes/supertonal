/*
  ==============================================================================

	PluginAudioParameters.h
	Created: 29 Dec 2023 3:38:11pm
	Author:  paulm

  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>
#include <JuceHeader.h>

namespace apvts
{
	// https://www.desmos.com/calculator/qkc6naksy5
	// Good for volume and frequency
	static inline juce::NormalisableRange<float> makeLogarithmicRange(float rangeStart, float rangeEnd, float intervalValue, float exponent = 6.0f)
	{
		juce::NormalisableRange<float> normalisableRange = {
			rangeStart, 
			rangeEnd,
			[=](float start, float end, float normalised)
			{
				return start + (std::exp2(normalised * exponent) - 1) * (end - start) / (std::exp2(exponent) - 1);
			},
			[=](float start, float end, float unnormalised)
			{
				return std::log2(((unnormalised - start) / (end - start) * (std::exp2(exponent) - 1)) + 1) / exponent;
			} };
		normalisableRange.interval = intervalValue;
		return normalisableRange;
	}

	static inline juce::NormalisableRange<float> makeDecibelRange(float rangeStart, float rangeEnd, float rangeInterval)
	{
		juce::NormalisableRange<float> range = {
			rangeStart,
			rangeEnd, 
			[=](float min, float max, float normalised) // convertFrom0to1
			{
				return normalised * (max - min) + min; 
			},
			[=](float min, float max, float unnormalised) // convertTo0to1
			{
				return (unnormalised - min) / (max - min); 
			} };
		range.interval = rangeInterval; // Setting the interval for the range
		return range;
	}

	static const std::string identifier = "apvts";
	static constexpr int version = 3;

	static const std::string impulseResponseFileFullPathNameId = "ir_full_path";

	// Defaults

	static const float defaultIntervalValue = 0.001;
	static const float defaultValueOff = 0.0f;
	static const float defaultValueHalf = 0.5f;
	static const float defaultValueOn = 1.0f;

	static const juce::NormalisableRange<float> zeroToOneLinearNormalisableRange = juce::NormalisableRange<float>(
		0.0f,
		1.0f,
		defaultIntervalValue);

	static const juce::NormalisableRange<float> negativeOneToOneLinearNormalisableRange = juce::NormalisableRange<float>(
		-1.0f,
		1.0f,
		defaultIntervalValue);

	static const juce::NormalisableRange<float> zeroToOneLogarithmicNormalisableRange = makeLogarithmicRange(
		0.0f,
		1.0f,
		defaultIntervalValue);

	// Wave Shaper

	static const std::string waveshaperComponentId = "wave_shaper";

	static const std::string hyperbolicTangentWaveShaperId = "htan";
	static const std::string softClipWaveShaperId = "soft";
	static const std::string arctangentWaveShaperId = "atan";
	static const std::string cubicNonLineartyWaveShaperId = "cube";
	static const std::string hardClipWaveShaperId = "hard";
	static const std::string exponentialWaveShaperId = "expo";

	static const std::vector<std::string> waveShaperIds = {
		hyperbolicTangentWaveShaperId,
		softClipWaveShaperId,
		arctangentWaveShaperId,
		cubicNonLineartyWaveShaperId,
		hardClipWaveShaperId,
		exponentialWaveShaperId,
	};

	static const std::map<std::string, float (*) (float)> waveShaperIdToFunctionMap = {
		{hyperbolicTangentWaveShaperId, [](float x) { return std::tanh(x); }},
		{softClipWaveShaperId, [](float x) { return x / (std::abs(x) + 1.0f); }},
		{arctangentWaveShaperId, [](float x) { return std::atan(x); }},
		{cubicNonLineartyWaveShaperId, [](float x) { return x - (1.0f / 3.0f) * std::pow(x, 3.0f); }},
		{hardClipWaveShaperId, [](float x) { return std::max(-1.0f, std::min(1.0f, x)); }},
		{exponentialWaveShaperId, [](float x) { return std::exp(x) - 1; }}
	};

	// Series/Parallel

	static const std::string stageModeComponentId = "mode";

	static const std::string seriesId = "series";
	static const std::string parallelId = "parallel";

	static const std::vector<std::string> stageModeIds = {
		seriesId,
		parallelId,
	};

	// BYPASS

	static const std::string onComponentId = "on";
	static const std::string biasId = "bias";

	// GAIN

	static constexpr float gainLinearMinimumValue = 0.001f;
	static constexpr float gainLinearMaximumValue = 64.0f;
	static constexpr float gainLinearDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> gainLinearNormalisableRange =
		makeLogarithmicRange(
			gainLinearMinimumValue,
			gainLinearMaximumValue,
			defaultIntervalValue);

	static constexpr float gainDecibelsMinimumValue = -128.0f;
	static constexpr float gainDeciblesMaximumValue = 64.0f;
	static constexpr float gainDeciblesDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> gainDecibelsNormalisableRange = makeDecibelRange(
		gainDecibelsMinimumValue,
		gainDeciblesMaximumValue,
		defaultIntervalValue);

	static const juce::NormalisableRange<float> gainDecibelsNegativeNormalisableRange = makeDecibelRange(
		gainDecibelsMinimumValue,
		gainDeciblesDefaultValue,
		defaultIntervalValue);

	// COMPRESSION

	static constexpr float ratioMinimumValue = 1.0f;
	static constexpr float ratioMaximumValue = 100.0f;
	static constexpr float ratioDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> ratioNormalizableRange = makeLogarithmicRange(
		ratioMinimumValue,
		ratioMaximumValue,
		defaultIntervalValue);

	static constexpr float attackMsMinimumValue = 0.0f;
	static constexpr float attackMsMaximumValue = 1000.0f;
	static constexpr float attackMsDefaultValue = 3.0f;
	static const juce::NormalisableRange<float> attackNormalisableRange = makeLogarithmicRange(
		attackMsMinimumValue, 
		attackMsMaximumValue, 
		defaultIntervalValue);

	static constexpr float releaseMsMinimumValue = 0.0f;
	static constexpr float releaseMsMaximumValue = 10000.0f;
	static constexpr float releaseMsDefaultValue = 500.0f;
	static const juce::NormalisableRange<float> releaseMsNormalisableRange = makeLogarithmicRange(
		releaseMsMinimumValue, 
		releaseMsMaximumValue, 
		defaultIntervalValue);

	// TIME

	static constexpr float fractionalTimeMinimumValue = 0.001f;
	static constexpr float fractionalTimeMaximumValue = 64.0f;
	static constexpr float fractionalTimeDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> fractionalTimeNormalizableRange = makeLogarithmicRange(
		fractionalTimeMinimumValue,
		fractionalTimeMaximumValue,
		defaultIntervalValue);

	static float calculateSamplesForBpmFractionAndRate(
		float beatsPerMinute, // 120
		float fractionOfBeat, // 4.0f
		const float samplesPerSecond // 48,000
	) {
		float beatDurationSeconds = (60.0f / beatsPerMinute); // 0.5
		float samplesPerBeat = beatDurationSeconds * samplesPerSecond; // 24,000
		return samplesPerBeat / fractionOfBeat; // 200;
	}

	// EQ

	static const std::string highPassEqualizationComponentId = "high-pass";
	static const std::string midPeakEqualizationComponentId = "mid-peak";
	static const std::string highShelfEqualizationComponentId = "high-shelf";
	static const std::string lowPassEqualizationComponentId = "low-pass";

	static constexpr float lowShelfFrequencyDefaultValue = 20.0f;
	static constexpr float midPeakFrequencyDefaultValue = 1000.0f;
	static constexpr float highShelfFrequencyDefaultValue = 15000.0f;
	static constexpr float lowPassFrequencyDefaultValue = 20000.0f;

	static const std::map<std::string, float> equalizationTypeIdToDefaultFrequencyMap = {
		{highPassEqualizationComponentId,lowShelfFrequencyDefaultValue},
		{midPeakEqualizationComponentId,midPeakFrequencyDefaultValue},
		{highShelfEqualizationComponentId,highShelfFrequencyDefaultValue},
		{lowPassEqualizationComponentId,lowPassFrequencyDefaultValue},
	};

	static constexpr float qualityDefaultValue = 0.70710678118654752440L; // 1 / sqrt(2)
	static constexpr float qualityMinimumValue = 0.001f;
	static constexpr float qualityMaximumValue = 10.f;
	static const juce::NormalisableRange<float> qualityNormalisableRange = makeLogarithmicRange(
		qualityMinimumValue, 
		qualityMaximumValue, 
		defaultIntervalValue);

	static constexpr float frequencyMinimumValue = 15.0f;
	static constexpr float frequencyMaximumValue = 20000.0f;
	static const juce::NormalisableRange<float> frequencyNormalisableRange = makeLogarithmicRange(
		frequencyMinimumValue, 
		frequencyMaximumValue, 
		defaultIntervalValue);

	// Limiter 

	static constexpr float limiterThresholdDefaultValue = -1.0f;
	static constexpr float limiterReleaseDefaultValue = 100.0f;

	// DELAY

	static constexpr float delayTimeMsMaximumValue = 10000.0f;

	// PHASER

	static constexpr float phaserCenterFrequencyDefaultValue = 1000.0f;

	// Rate to BPM

	static float clampedValueForFractionOfBeat(
		float beatsPerMinute, 
		float fractionOfBeat, float minimumValue = 0.000f, float maximumValue = 99.999f) 
	{
		float noteDurationSeconds = 60.0f / beatsPerMinute * fractionOfBeat;
		float frequencyHz = 1.0f / noteDurationSeconds;
		return std::clamp(frequencyHz, minimumValue, maximumValue);
	}

	// Tube screamer

	static constexpr float tubeScreamerDiodeTypeMinimumValue = 0.0f;
	static constexpr float tubeScreamerDiodeTypeMaximumValue = 2.0f;
	static constexpr float tubeScreamerDiodeTypeInterval = 1.0;
	static constexpr float tubeScreamerDiodeTypeDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> tubeScreamerDiodeTypeNormalizableRange = juce::NormalisableRange<float>(
		tubeScreamerDiodeTypeMinimumValue,
		tubeScreamerDiodeTypeMaximumValue,
		tubeScreamerDiodeTypeInterval);

	static constexpr float tubeScreamerDiodeCountMinimumValue = 1.0f;
	static constexpr float tubeScreamerDiodeCountMaximumValue = 3.0f;
	static constexpr float tubeScreamerDiodeCountInterval = 1.0;
	static constexpr float tubeScreamerDiodeCountDefaultValue = 2.0f;
	static const juce::NormalisableRange<float> tubeScreamerDiodeCountNormalizableRange = juce::NormalisableRange<float>(
		tubeScreamerDiodeCountMinimumValue,
		tubeScreamerDiodeCountMaximumValue,
		tubeScreamerDiodeCountInterval);

	enum class ParameterEnum {
		BYPASS_ON,

		NOISE_GATE_THRESHOLD,
		NOISE_GATE_ATTACK,
		NOISE_GATE_RATIO,
		NOISE_GATE_RELEASE,

		PRE_COMPRESSOR_THRESHOLD,
		PRE_COMPRESSOR_ATTACK,
		PRE_COMPRESSOR_RATIO,
		PRE_COMPRESSOR_RELEASE,

		TUBE_SCREAMER_ON,
		TUBE_SCREAMER_DRIVE,
		TUBE_SCREAMER_LEVEL,
		TUBE_SCREAMER_DIODE_TYPE,
		TUBE_SCREAMER_DIODE_COUNT,

		MOUSE_DRIVE_ON,
		MOUSE_DRIVE_DISTORTION,
		MOUSE_DRIVE_VOLUME,

		STAGE_MODE,

		STAGE1_ON,
		STAGE1_INPUT_GAIN,
		STAGE1_WAVE_SHAPER,
		STAGE1_OUTPUT_GAIN,
		STAGE1_DRY_WET,
		
		STAGE2_ON,
		STAGE2_INPUT_GAIN,
		STAGE2_WAVE_SHAPER,
		STAGE2_OUTPUT_GAIN,
		STAGE2_DRY_WET,
		
		STAGE3_ON,
		STAGE3_INPUT_GAIN,
		STAGE3_WAVE_SHAPER,
		STAGE3_OUTPUT_GAIN,
		STAGE3_DRY_WET,
		
		STAGE4_ON,
		STAGE4_INPUT_GAIN,
		STAGE4_WAVE_SHAPER,
		STAGE4_OUTPUT_GAIN,
		STAGE4_DRY_WET,
		
		BIAS,
		
		POST_COMPRESSOR_THRESHOLD,
		POST_COMPRESSOR_ATTACK,
		POST_COMPRESSOR_RATIO,
		POST_COMPRESSOR_RELEASE,
		POST_COMPRESSOR_GAIN,
		
		HIGH_PASS_FREQUENCY,
		HIGH_PASS_Q,
		
		MID_PEAK_FREQUENCY,
		MID_PEAK_Q,
		MID_PEAK_GAIN,
		
		HIGH_SHELF_FREQUENCY,
		HIGH_SHELF_Q,
		HIGH_SHELF_GAIN,

		LOW_PASS_FREQUENCY,
		LOW_PASS_Q,
		
		DELAY_TIME_FRACTIONAL_DENOMINATOR,
		DELAY_FEEDBACK,
		DELAY_DRY_WET,

		CHORUS_FRACTION_OF_BEAT,
		CHORUS_DEPTH,
		CHORUS_CENTER_DELAY_FRACTION_OF_BEAT,
		CHORUS_FEEDBACK,
		CHORUS_MIX,
		
		PHASER_RATE_FRACTION_OF_BEAT,
		PHASER_DEPTH,
		PHASER_CENTER_FREQUENCY,
		PHASER_FEEDBACK,
		PHASER_MIX,
		
		REVERB_ON,
		REVERB_SIZE,
		REVERB_DAMPING,
		REVERB_MIX,
		REVERB_WIDTH,

		CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON,
		CABINET_OUTPUT_GAIN,

		LIMITER_ON,
		LIMITER_THRESHOLD,
		LIMITER_RELEASE,

		OUTPUT_GAIN
	};

	static const std::string bypassId = "bypass_on";

	static const std::string noiseGateThresholdId = "noise_gate_threshold";
	static const std::string noiseGateAttackId = "noise_gate_attack";
	static const std::string noiseGateRatioId = "noise_gate_ratio";
	static const std::string noiseGateReleaseId = "noise_gate_release";
	static const std::string noiseGateGainId = "noise_gate_gain";

	static const std::string preCompressorThresholdId = "pre_comp_thresh";
	static const std::string preCompressorAttackId = "pre_comp_attack";
	static const std::string preCompressorRatioId = "pre_comp_ratio";
	static const std::string preCompressorReleaseId = "pre_comp_release";

	static const std::string tubeScreamerOnId = "tube_screamer_on";
	static const std::string tubeScreamerDriveId = "tube_screamer_drive";
	static const std::string tubeScreamerLevelId = "tube_screamer_level";
	static const std::string tubeScreamerDiodeTypeId = "tube_screamer_diode_type";
	static const std::string tubeScreamerDiodeCountId = "tube_screamer_diode_count";

	static const std::string mouseDriveOnId = "mouse_drive_on";
	static const std::string mouseDriveDistortionId = "mouse_drive_distortion";
	static const std::string mouseDriveVolumeId = "mouse_drive_volume";

	static const std::string stageModeId = "mode";

	static const std::string stage1OnId = "stage_1_on";
	static const std::string stage1InputGainId = "stage_1_input_gain";
	static const std::string stage1WaveShaperId = "stage_1_wave_shaper";
	static const std::string stage1OutputGainId = "stage_1_output_gain";
	static const std::string stage1DryWetId = "stage_1_mix";

	static const std::string stage2OnId = "stage_2_on";
	static const std::string stage2InputGainId = "stage_2_input_gain";
	static const std::string stage2WaveShaperId = "stage_2_wave_shaper";
	static const std::string stage2OutputGainId = "stage_2_output_gain";
	static const std::string stage2DryWetId = "stage_2_mix";

	static const std::string stage3OnId = "stage_3_on";
	static const std::string stage3InputGainId = "stage_3_input_gain";
	static const std::string stage3WaveShaperId = "stage_3_wave_shaper";
	static const std::string stage3OutputGainId = "stage_3_output_gain";
	static const std::string stage3DryWetId = "stage_3_mix";

	static const std::string stage4OnId = "stage_4_on";
	static const std::string stage4InputGainId = "stage_4_input_gain";
	static const std::string stage4WaveShaperId = "stage_4_wave_shaper";
	static const std::string stage4OutputGainId = "stage_4_output_gain";
	static const std::string stage4DryWetId = "stage_4_mix";

	static const std::string postCompressorThresholdId = "post_comp_thresh";
	static const std::string postCompressorAttackId = "post_comp_attack";
	static const std::string postCompressorRatioId = "post_comp_ratio";
	static const std::string postCompressorReleaseId = "post_comp_release";
	static const std::string postCompressorGainId = "post_comp_gain";

	static const std::string highPassFrequencyId = "high-pass_frequency";
	static const std::string highPassQId = "high-pass_q";

	static const std::string midPeakFrequencyId = "mid-peak_frequency";
	static const std::string midPeakQId = "mid-peak_q";
	static const std::string midPeakGainId = "mid-peak_gain";

	static const std::string highShelfFrequencyId = "high-shelf_frequency";
	static const std::string highShelfQId = "high-shelf_q";
	static const std::string highShelfGainId = "high-shelf_gain";

	static const std::string lowPassFrequencyId = "low-pass_frequency";
	static const std::string lowPassQId = "low-pass_q";

	static const std::string delayTimeFractionalDenominatorId = "delay_per_beat";
	static const std::string delayFeedbackId = "delay_feedback";
	static const std::string delayDryWetId = "delay_mix";

	static const std::string chorusFractionOfBeatId = "chorus_per_beat";
	static const std::string chorusDepthId = "chorus_depth";
	static const std::string chorusCenterDelayFractionOfBeatId = "chorus_delay_per_beat";
	static const std::string chorusFeedbackId = "chorus_feedback";
	static const std::string chorusMixId = "chorus_mix";

	static const std::string phaserRateFractionOfBeatId = "phaser_per_beat";
	static const std::string phaserDepthId = "phaser_depth";
	static const std::string phaserCenterFrequencyId = "phaser_center_freq";
	static const std::string phaserFeedbackId = "phaser_feedback";
	static const std::string phaserMixId = "phaser_mix";

	static const std::string reverbOnId = "room_on";
	static const std::string reverbSizeId = "room_size";
	static const std::string reverbDampingId = "room_damping";
	static const std::string reverbMixId = "room_mix";
	static const std::string reverbWidthId = "room_width";

	static const std::string cabinetImpulseResponseConvolutionOnId = "cab_on";
	static const std::string cabinetImpulseResponseConvolutionFileId = "cab_file";
	static const std::string cabinetGainId = "cabinet_gain";

	static const std::string limiterOnId = "limiter_on";
	static const std::string limiterThresholdId = "limiter_threshold";
	static const std::string limiterReleaseId = "limiter_release";

	static const std::string outputGainId = "output_gain";

	static const std::map<std::string, ParameterEnum> parameterIdToEnumMap {
		{bypassId, ParameterEnum::BYPASS_ON},

		{noiseGateThresholdId, ParameterEnum::NOISE_GATE_THRESHOLD},
		{noiseGateAttackId, ParameterEnum::NOISE_GATE_ATTACK},
		{noiseGateRatioId, ParameterEnum::NOISE_GATE_RATIO},
		{noiseGateReleaseId, ParameterEnum::NOISE_GATE_RELEASE},
		
		{preCompressorThresholdId, ParameterEnum::PRE_COMPRESSOR_THRESHOLD},
		{preCompressorAttackId, ParameterEnum::PRE_COMPRESSOR_ATTACK},
		{preCompressorRatioId, ParameterEnum::PRE_COMPRESSOR_RATIO},
		{preCompressorReleaseId, ParameterEnum::PRE_COMPRESSOR_RELEASE},

		{tubeScreamerOnId, ParameterEnum::TUBE_SCREAMER_ON},
		{tubeScreamerDriveId, ParameterEnum::TUBE_SCREAMER_DRIVE},
		{tubeScreamerLevelId, ParameterEnum::TUBE_SCREAMER_LEVEL},
		{tubeScreamerDiodeTypeId, ParameterEnum::TUBE_SCREAMER_DIODE_TYPE},
		{tubeScreamerDiodeCountId, ParameterEnum::TUBE_SCREAMER_DIODE_COUNT},

		{mouseDriveOnId, ParameterEnum::MOUSE_DRIVE_ON},
		{mouseDriveDistortionId, ParameterEnum::MOUSE_DRIVE_DISTORTION},
		{mouseDriveVolumeId, ParameterEnum::MOUSE_DRIVE_VOLUME},

		{stageModeId, ParameterEnum::STAGE_MODE},
		
		{stage1OnId, ParameterEnum::STAGE1_ON},
		{stage1InputGainId, ParameterEnum::STAGE1_INPUT_GAIN},
		{stage1WaveShaperId, ParameterEnum::STAGE1_WAVE_SHAPER},
		{stage1OutputGainId, ParameterEnum::STAGE1_OUTPUT_GAIN},
		{stage1DryWetId, ParameterEnum::STAGE1_DRY_WET},
		
		{stage2OnId, ParameterEnum::STAGE2_ON},
		{stage2InputGainId, ParameterEnum::STAGE2_INPUT_GAIN},
		{stage2WaveShaperId, ParameterEnum::STAGE2_WAVE_SHAPER},
		{stage2OutputGainId, ParameterEnum::STAGE2_OUTPUT_GAIN},
		{stage2DryWetId, ParameterEnum::STAGE2_DRY_WET},
		
		{stage3OnId, ParameterEnum::STAGE3_ON},
		{stage3InputGainId, ParameterEnum::STAGE3_INPUT_GAIN},
		{stage3WaveShaperId, ParameterEnum::STAGE3_WAVE_SHAPER},
		{stage3OutputGainId, ParameterEnum::STAGE3_OUTPUT_GAIN},
		{stage3DryWetId, ParameterEnum::STAGE3_DRY_WET},
		
		{stage4OnId, ParameterEnum::STAGE4_ON},
		{stage4InputGainId, ParameterEnum::STAGE4_INPUT_GAIN},
		{stage4WaveShaperId, ParameterEnum::STAGE4_WAVE_SHAPER},
		{stage4OutputGainId, ParameterEnum::STAGE4_OUTPUT_GAIN},
		{stage4DryWetId, ParameterEnum::STAGE4_DRY_WET},
		
		{biasId, ParameterEnum::BIAS},
		
		{postCompressorThresholdId, ParameterEnum::POST_COMPRESSOR_THRESHOLD},
		{postCompressorAttackId, ParameterEnum::POST_COMPRESSOR_ATTACK},
		{postCompressorRatioId, ParameterEnum::POST_COMPRESSOR_RATIO},
		{postCompressorReleaseId, ParameterEnum::POST_COMPRESSOR_RELEASE},
		{postCompressorGainId, ParameterEnum::POST_COMPRESSOR_GAIN},
		
		{highPassFrequencyId, ParameterEnum::HIGH_PASS_FREQUENCY},
		{highPassQId, ParameterEnum::HIGH_PASS_Q},
		
		{midPeakFrequencyId, ParameterEnum::MID_PEAK_FREQUENCY},
		{midPeakQId, ParameterEnum::MID_PEAK_Q},
		{midPeakGainId, ParameterEnum::MID_PEAK_GAIN},
		
		{highShelfFrequencyId, ParameterEnum::HIGH_SHELF_FREQUENCY},
		{highShelfQId, ParameterEnum::HIGH_SHELF_Q},
		{highShelfGainId, ParameterEnum::HIGH_SHELF_GAIN},

		{lowPassFrequencyId, ParameterEnum::LOW_PASS_FREQUENCY},
		{lowPassQId, ParameterEnum::LOW_PASS_Q},
		
		{cabinetImpulseResponseConvolutionOnId, ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON},
		
		{cabinetGainId, ParameterEnum::CABINET_OUTPUT_GAIN},
		
		{limiterOnId, ParameterEnum::LIMITER_ON},
		{limiterThresholdId, ParameterEnum::LIMITER_THRESHOLD},
		{limiterReleaseId, ParameterEnum::LIMITER_RELEASE},

		{delayTimeFractionalDenominatorId, ParameterEnum::DELAY_TIME_FRACTIONAL_DENOMINATOR},
		{delayFeedbackId, ParameterEnum::DELAY_FEEDBACK},
		{delayDryWetId, ParameterEnum::DELAY_DRY_WET},

		{chorusFractionOfBeatId, ParameterEnum::CHORUS_FRACTION_OF_BEAT},
		{chorusDepthId, ParameterEnum::CHORUS_DEPTH},
		{chorusCenterDelayFractionOfBeatId, ParameterEnum::CHORUS_CENTER_DELAY_FRACTION_OF_BEAT},
		{chorusFeedbackId, ParameterEnum::CHORUS_FEEDBACK},
		{chorusMixId, ParameterEnum::CHORUS_MIX},

		{phaserRateFractionOfBeatId, ParameterEnum::PHASER_RATE_FRACTION_OF_BEAT},
		{phaserDepthId, ParameterEnum::PHASER_DEPTH},
		{phaserCenterFrequencyId, ParameterEnum::PHASER_CENTER_FREQUENCY},
		{phaserFeedbackId, ParameterEnum::PHASER_FEEDBACK},
		{phaserMixId, ParameterEnum::PHASER_MIX},

		{reverbOnId, ParameterEnum::REVERB_ON},
		{reverbSizeId, ParameterEnum::REVERB_SIZE},
		{reverbDampingId, ParameterEnum::REVERB_DAMPING},
		{reverbMixId, ParameterEnum::REVERB_MIX},
		{reverbWidthId, ParameterEnum::REVERB_WIDTH},

		{ outputGainId, ParameterEnum::OUTPUT_GAIN},
	};

	static const std::map<std::string, float> defaults = {

	};

	static float getDefaultValue(const std::string key, const float fallbackValue)
	{
		auto it = defaults.find(key);
		if (it != defaults.end())
		{
			return it->second;
		}
		else
		{
			return fallbackValue;
		}
	}
}