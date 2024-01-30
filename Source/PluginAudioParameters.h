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
	static inline juce::NormalisableRange<float> makeLogarithmicRange(float rangeStart, float rangeEnd, float intervalValue)
	{
		juce::NormalisableRange<float> normalisableRange = {
			rangeStart, 
			rangeEnd,
			[=](float start, float end, float normalised)
			{
				return start + (std::exp2(normalised * 6.0f) - 1) * (end - start) / (std::exp2(6.0f) - 1);
			},
			[=](float start, float end, float unnormalised)
			{
				return std::log2(((unnormalised - start) / (end - start) * (std::exp2(6.0f) - 1)) + 1) / 6.0f;
			} 
		};
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
			} 
		};
		range.interval = rangeInterval; // Setting the interval for the range
		return range;
	}

	static const std::string identifier = "apvts";
	static constexpr int version = 3;

	static const std::string impulseResponseFileFullPathNameId = "ir_full_path";

	static constexpr float sampleRateAssumption = 441000.0f;

	// Defaults

	static const float defaultIntervalValue = 0.01;
	static const float defaultValueOff = 0.0f;
	static const float defaultValueQuarter = 0.25f;
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
	static constexpr float attackMsDefaultValue = 5.0f;
	static const juce::NormalisableRange<float> attackNormalisableRange = makeLogarithmicRange(
		attackMsMinimumValue, 
		attackMsMaximumValue, 
		defaultIntervalValue);

	static constexpr float releaseMsMinimumValue = 0.0f;
	static constexpr float releaseMsMaximumValue = 10000.0f;
	static constexpr float releaseMsDefaultValue = 100.0f;
	static const juce::NormalisableRange<float> releaseMsNormalisableRange = makeLogarithmicRange(
		releaseMsMinimumValue, 
		releaseMsMaximumValue, 
		defaultIntervalValue);

	// TIME

	static constexpr float fractionalTimeMinimumValue = 0.001f;
	static constexpr float fractionalTimeMaximumValue = 64.0f;
	static constexpr float fractionalTimeDefaultValue = 4.0f;
	static const juce::NormalisableRange<float> fractionalTimeNormalizableRange = makeLogarithmicRange(
		fractionalTimeMinimumValue,
		fractionalTimeMaximumValue,
		defaultIntervalValue);

	static float calculateSamplesForBpmFractionAndRate(
		float beatsPerMinute, // ex. 120
		float fractionOfBeat, // ex. 4.0f
		const float samplesPerSecond // ex. 48,000
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
		INPUT_GAIN,

		NOISE_GATE_THRESHOLD,
		NOISE_GATE_ATTACK,
		NOISE_GATE_RATIO,
		NOISE_GATE_RELEASE,

		PRE_COMPRESSOR_ON,
		PRE_COMPRESSOR_THRESHOLD,
		PRE_COMPRESSOR_ATTACK,
		PRE_COMPRESSOR_RATIO,
		PRE_COMPRESSOR_RELEASE,
		PRE_COMPRESSOR_BLEND,
		PRE_COMPRESSOR_GAIN,

		PRE_EQUALISER_ON,
		PRE_EQUALISER_100_GAIN,
		PRE_EQUALISER_200_GAIN,
		PRE_EQUALISER_400_GAIN,
		PRE_EQUALISER_800_GAIN,
		PRE_EQUALISER_1600_GAIN,
		PRE_EQUALISER_3200_GAIN,
		PRE_EQUALISER_6400_GAIN,
		PRE_EQUALISER_LEVEL_GAIN,

		TUBE_SCREAMER_ON,
		TUBE_SCREAMER_DRIVE,
		TUBE_SCREAMER_LEVEL,
		TUBE_SCREAMER_DIODE_TYPE,
		TUBE_SCREAMER_DIODE_COUNT,
		TUBE_SCREAMER_TONE,

		MOUSE_DRIVE_ON,
		MOUSE_DRIVE_DISTORTION,
		MOUSE_DRIVE_VOLUME,
		MOUSE_DRIVE_FILTER,

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
		
		AMP_RESONANCE_DB,
		AMP_BASS_DB,
		AMP_MIDDLE_DB,
		AMP_TREBLE_DB,
		AMP_PRESENCE_DB,
		
		DELAY_ON,
		DELAY_TIME_FRACTIONAL_DENOMINATOR,
		DELAY_FEEDBACK,
		DELAY_DRY_WET,

		CHORUS_ON,
		CHORUS_FRACTION_OF_BEAT,
		CHORUS_DEPTH,
		CHORUS_CENTER_DELAY_FRACTION_OF_BEAT,
		CHORUS_FEEDBACK,
		CHORUS_MIX,
		
		PHASER_ON,
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

		INSTRUMENT_EQUALISER_LOW_PASS_ON,
		INSTRUMENT_EQUALISER_LOW_PASS_FREQUENCY,

		INSTRUMENT_EQUALISER_LOW_PEAK_ON,
		INSTRUMENT_EQUALISER_LOW_PEAK_FREQUENCY,
		INSTRUMENT_EQUALISER_LOW_PEAK_GAIN,
		INSTRUMENT_EQUALISER_LOW_PEAK_QUALITY,

		INSTRUMENT_EQUALISER_LOW_MID_PEAK_ON,
		INSTRUMENT_EQUALISER_LOW_MID_PEAK_FREQUENCY,
		INSTRUMENT_EQUALISER_LOW_MID_PEAK_GAIN,
		INSTRUMENT_EQUALISER_LOW_MID_PEAK_QUALITY,

		INSTRUMENT_EQUALISER_HIGH_MID_PEAK_ON,
		INSTRUMENT_EQUALISER_HIGH_MID_PEAK_FREQUENCY,
		INSTRUMENT_EQUALISER_HIGH_MID_PEAK_GAIN,
		INSTRUMENT_EQUALISER_HIGH_MID_PEAK_QUALITY,

		INSTRUMENT_EQUALISER_HIGH_PEAK_ON,
		INSTRUMENT_EQUALISER_HIGH_PEAK_FREQUENCY,
		INSTRUMENT_EQUALISER_HIGH_PEAK_GAIN,
		INSTRUMENT_EQUALISER_HIGH_PEAK_QUALITY,

		INSTRUMENT_EQUALISER_HIGH_PASS_ON,
		INSTRUMENT_EQUALISER_HIGH_PASS_FREQUENCY,

		LIMITER_ON,
		LIMITER_THRESHOLD,
		LIMITER_RELEASE,

		OUTPUT_GAIN,
		BYPASS_ON
	};

	static const std::string bypassId = "bypass_on";

	static const std::string inputGainId = "input_gain";

	static const std::string noiseGateThresholdId = "noise_gate_threshold";
	static const std::string noiseGateAttackId = "noise_gate_attack";
	static const std::string noiseGateRatioId = "noise_gate_ratio";
	static const std::string noiseGateReleaseId = "noise_gate_release";
	static const std::string noiseGateGainId = "noise_gate_gain";

	static const std::string preCompressorOnId = "pre_compressor_on";
	static const std::string preCompressorThresholdId = "pre_comp_thresh";
	static const std::string preCompressorAttackId = "pre_comp_attack";
	static const std::string preCompressorRatioId = "pre_comp_ratio";
	static const std::string preCompressorReleaseId = "pre_comp_release";
	static const std::string preCompressorBlendId = "pre_comp_blend";
	static const std::string preCompressorGainId = "pre_comp_gain";

	static const std::string tubeScreamerOnId = "tube_screamer_on";
	static const std::string tubeScreamerDriveId = "tube_screamer_drive";
	static const std::string tubeScreamerLevelId = "tube_screamer_level";
	static const std::string tubeScreamerDiodeTypeId = "tube_screamer_diode_type";
	static const std::string tubeScreamerDiodeCountId = "tube_screamer_diode_count";
	static const std::string tubeScreamerToneId = "tube_screamer_tone";

	static const std::string mouseDriveOnId = "mouse_drive_on";
	static const std::string mouseDriveDistortionId = "mouse_drive_distortion";
	static const std::string mouseDriveVolumeId = "mouse_drive_volume";
	static const std::string mouseDriveFilterId = "mouse_drive_filter";

	static const std::string preEqualiserOnId = "pre_eq_on";
	static const std::string preEqualiser100GainId = "pre_eq_100_gain";
	static const std::string preEqualiser200GainId = "pre_eq_200_gain";
	static const std::string preEqualiser400GainId = "pre_eq_400_gain";
	static const std::string preEqualiser800GainId = "pre_eq_800_gain";
	static const std::string preEqualiser1600GainId = "pre_eq_1600_gain";
	static const std::string preEqualiser3200GainId = "pre_eq_3200_gain";
	static const std::string preEqualiser6400GainId = "pre_eq_6400_gain";
	static const std::string preEqualiserLevelId = "pre_eq_level_gain";

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

	static const std::string ampResonanceDbId = "amp_resonance";
	static const std::string ampBassDbId = "amp_bass";
	static const std::string ampMiddleDbId = "amp_middle";
	static const std::string ampTrebleDbId = "amp_treble";
	static const std::string ampPresenceDbId = "amp_presence";

	static const std::string delayOnId = "delay_on";
	static const std::string delayTimeFractionalDenominatorId = "delay_per_beat";
	static const std::string delayFeedbackId = "delay_feedback";
	static const std::string delayDryWetId = "delay_mix";

	static const std::string chorusOnId = "chorus_on";
	static const std::string chorusFractionOfBeatId = "chorus_per_beat";
	static const std::string chorusDepthId = "chorus_depth";
	static const std::string chorusCenterDelayFractionOfBeatId = "chorus_delay_per_beat";
	static const std::string chorusFeedbackId = "chorus_feedback";
	static const std::string chorusMixId = "chorus_mix";

	static const std::string phaserOnId = "phaser_on";
	static const std::string phaserRateFractionOfBeatId = "phaser_per_beat";
	static const std::string phaserDepthId = "phaser_depth";
	static const std::string phaserCenterFrequencyId = "phaser_center_freq";
	static const std::string phaserFeedbackId = "phaser_feedback";
	static const std::string phaserMixId = "phaser_mix";

	static const std::string roomOnId = "room_on";
	static const std::string roomSizeId = "room_size";
	static const std::string roomDampingId = "room_damping";
	static const std::string roomMixId = "room_mix";
	static const std::string roomWidthId = "room_width";

	static const std::string cabinetImpulseResponseConvolutionOnId = "cab_on";
	static const std::string cabinetImpulseResponseConvolutionFileId = "cab_file";
	static const std::string cabinetGainId = "cabinet_gain";

	static const std::string instrumentEqualiserLowPassOnId = "eq_low_pass_on";
	static const std::string instrumentEqualiserLowPassFrequencyId = "eq_low_pass_freq";
	
	static const std::string instrumentEqualiserLowPeakOnId = "eq_low_peak_on";
	static const std::string instrumentEqualiserLowPeakFrequencyId = "eq_low_peak_freq";
	static const std::string instrumentEqualiserLowPeakGainId = "eq_low_peak_gain";
	static const std::string instrumentEqualiserLowPeakQualityId = "eq_low_peak_q";
	
	static const std::string instrumentEqualiserLowMidPeakOnId = "eq_low_mid_peak_on";
	static const std::string instrumentEqualiserLowMidPeakFrequencyId = "eq_low_mid_peak_freq";
	static const std::string instrumentEqualiserLowMidPeakGainId = "eq_low_mid_peak_gain";
	static const std::string instrumentEqualiserLowMidPeakQualityId = "eq_low_mid_peak_q";
	
	static const std::string instrumentEqualiserHighMidPeakOnId = "eq_high_mid_peak_on";
	static const std::string instrumentEqualiserHighMidPeakFrequencyId = "eq_high_mid_peak_freq";
	static const std::string instrumentEqualiserHighMidPeakGainId = "eq_high_mid_peak_gain";
	static const std::string instrumentEqualiserHighMidPeakQualityId = "eq_high_mid_peak_q";
	
	static const std::string instrumentEqualiserHighPeakOnId = "eq_high_peak_on";
	static const std::string instrumentEqualiserHighPeakFrequencyId = "eq_high_peak_freq";
	static const std::string instrumentEqualiserHighPeakGainId = "eq_high_peak_gain";
	static const std::string instrumentEqualiserHighPeakQualityId = "eq_high_peak_q";
	
	static const std::string instrumentEqualiserHighPassOnId = "eq_high_pass_on";
	static const std::string instrumentEqualiserHighPassFrequencyId = "eq_high_pass_freq";

	static const std::string limiterOnId = "limiter_on";
	static const std::string limiterThresholdId = "limiter_threshold";
	static const std::string limiterReleaseId = "limiter_release";

	static const std::string outputGainId = "output_gain";

	static const std::map<std::string, ParameterEnum> parameterIdToEnumMap {
		{inputGainId, ParameterEnum::INPUT_GAIN},

		{noiseGateThresholdId, ParameterEnum::NOISE_GATE_THRESHOLD},
		{noiseGateAttackId, ParameterEnum::NOISE_GATE_ATTACK},
		{noiseGateRatioId, ParameterEnum::NOISE_GATE_RATIO},
		{noiseGateReleaseId, ParameterEnum::NOISE_GATE_RELEASE},
		
		{preCompressorOnId, ParameterEnum::PRE_COMPRESSOR_ON},
		{preCompressorThresholdId, ParameterEnum::PRE_COMPRESSOR_THRESHOLD},
		{preCompressorAttackId, ParameterEnum::PRE_COMPRESSOR_ATTACK},
		{preCompressorRatioId, ParameterEnum::PRE_COMPRESSOR_RATIO},
		{preCompressorReleaseId, ParameterEnum::PRE_COMPRESSOR_RELEASE},
		{preCompressorBlendId, ParameterEnum::PRE_COMPRESSOR_BLEND},
		{preCompressorGainId, ParameterEnum::PRE_COMPRESSOR_GAIN},

		{tubeScreamerOnId, ParameterEnum::TUBE_SCREAMER_ON},
		{tubeScreamerDriveId, ParameterEnum::TUBE_SCREAMER_DRIVE},
		{tubeScreamerLevelId, ParameterEnum::TUBE_SCREAMER_LEVEL},
		{tubeScreamerDiodeTypeId, ParameterEnum::TUBE_SCREAMER_DIODE_TYPE},
		{tubeScreamerDiodeCountId, ParameterEnum::TUBE_SCREAMER_DIODE_COUNT},
		{tubeScreamerToneId, ParameterEnum::TUBE_SCREAMER_TONE},

		{mouseDriveOnId, ParameterEnum::MOUSE_DRIVE_ON},
		{mouseDriveDistortionId, ParameterEnum::MOUSE_DRIVE_DISTORTION},
		{mouseDriveVolumeId, ParameterEnum::MOUSE_DRIVE_VOLUME},
		{mouseDriveFilterId, ParameterEnum::MOUSE_DRIVE_FILTER},

		{preEqualiserOnId, ParameterEnum::PRE_EQUALISER_ON},
		{preEqualiser100GainId, ParameterEnum::PRE_EQUALISER_100_GAIN},
		{preEqualiser200GainId, ParameterEnum::PRE_EQUALISER_200_GAIN},
		{preEqualiser400GainId, ParameterEnum::PRE_EQUALISER_400_GAIN},
		{preEqualiser800GainId, ParameterEnum::PRE_EQUALISER_800_GAIN},
		{preEqualiser1600GainId, ParameterEnum::PRE_EQUALISER_1600_GAIN},
		{preEqualiser3200GainId, ParameterEnum::PRE_EQUALISER_3200_GAIN},
		{preEqualiser6400GainId, ParameterEnum::PRE_EQUALISER_6400_GAIN},
		{preEqualiserLevelId, ParameterEnum::PRE_EQUALISER_LEVEL_GAIN},

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
		
		{ampResonanceDbId, ParameterEnum::AMP_RESONANCE_DB},
		{ampBassDbId, ParameterEnum::AMP_BASS_DB},
		{ampMiddleDbId, ParameterEnum::AMP_MIDDLE_DB},
		{ampTrebleDbId, ParameterEnum::AMP_TREBLE_DB},
		{ampPresenceDbId, ParameterEnum::AMP_PRESENCE_DB},
		
		{cabinetImpulseResponseConvolutionOnId, ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON},
		
		{cabinetGainId, ParameterEnum::CABINET_OUTPUT_GAIN},
		
		{limiterOnId, ParameterEnum::LIMITER_ON},
		{limiterThresholdId, ParameterEnum::LIMITER_THRESHOLD},
		{limiterReleaseId, ParameterEnum::LIMITER_RELEASE},

		{delayOnId, ParameterEnum::DELAY_ON},
		{delayTimeFractionalDenominatorId, ParameterEnum::DELAY_TIME_FRACTIONAL_DENOMINATOR},
		{delayFeedbackId, ParameterEnum::DELAY_FEEDBACK},
		{delayDryWetId, ParameterEnum::DELAY_DRY_WET},

		{chorusOnId, ParameterEnum::CHORUS_ON},
		{chorusFractionOfBeatId, ParameterEnum::CHORUS_FRACTION_OF_BEAT},
		{chorusDepthId, ParameterEnum::CHORUS_DEPTH},
		{chorusCenterDelayFractionOfBeatId, ParameterEnum::CHORUS_CENTER_DELAY_FRACTION_OF_BEAT},
		{chorusFeedbackId, ParameterEnum::CHORUS_FEEDBACK},
		{chorusMixId, ParameterEnum::CHORUS_MIX},

		{phaserOnId, ParameterEnum::PHASER_ON},
		{phaserRateFractionOfBeatId, ParameterEnum::PHASER_RATE_FRACTION_OF_BEAT},
		{phaserDepthId, ParameterEnum::PHASER_DEPTH},
		{phaserCenterFrequencyId, ParameterEnum::PHASER_CENTER_FREQUENCY},
		{phaserFeedbackId, ParameterEnum::PHASER_FEEDBACK},
		{phaserMixId, ParameterEnum::PHASER_MIX},

		{instrumentEqualiserLowPassOnId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_ON },
		{instrumentEqualiserLowPassFrequencyId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_PASS_FREQUENCY },
		{instrumentEqualiserLowPeakOnId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_ON },
		{instrumentEqualiserLowPeakFrequencyId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_FREQUENCY },
		{instrumentEqualiserLowPeakGainId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_GAIN },
		{instrumentEqualiserLowPeakQualityId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_PEAK_QUALITY },
		{instrumentEqualiserLowMidPeakOnId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_ON },
		{instrumentEqualiserLowMidPeakFrequencyId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_FREQUENCY },
		{instrumentEqualiserLowMidPeakGainId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_GAIN },
		{instrumentEqualiserLowMidPeakQualityId, ParameterEnum::INSTRUMENT_EQUALISER_LOW_MID_PEAK_QUALITY },
		{instrumentEqualiserHighMidPeakOnId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_ON },
		{instrumentEqualiserHighMidPeakFrequencyId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_FREQUENCY },
		{instrumentEqualiserHighMidPeakGainId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_GAIN },
		{instrumentEqualiserHighMidPeakQualityId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_MID_PEAK_QUALITY },
		{instrumentEqualiserHighPeakOnId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_ON },
		{instrumentEqualiserHighPeakFrequencyId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_FREQUENCY },
		{instrumentEqualiserHighPeakGainId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_GAIN },
		{instrumentEqualiserHighPeakQualityId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PEAK_QUALITY },
		{instrumentEqualiserHighPassOnId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_ON },
		{instrumentEqualiserHighPassFrequencyId, ParameterEnum::INSTRUMENT_EQUALISER_HIGH_PASS_FREQUENCY },

		{roomOnId, ParameterEnum::REVERB_ON},
		{roomSizeId, ParameterEnum::REVERB_SIZE},
		{roomDampingId, ParameterEnum::REVERB_DAMPING},
		{roomMixId, ParameterEnum::REVERB_MIX},
		{roomWidthId, ParameterEnum::REVERB_WIDTH},

		{ outputGainId, ParameterEnum::OUTPUT_GAIN},
		{ bypassId, ParameterEnum::BYPASS_ON },
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
