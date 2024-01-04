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
	static const std::string identifier = "supertonal-apvts";
	static constexpr int version = 3;

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
		{exponentialWaveShaperId, [](float x) {return std::exp(x) - 1; }}
	};

	// Series/Parallel

	static const std::string modeComponentId = "mode";

	static const std::string seriesModeId = "series";
	static const std::string parallelModeId = "parallel";

	static const std::vector<std::string> modeIds = {
		seriesModeId,
		parallelModeId,
	};

	// BYPASS

	static const std::string onComponentId = "on";

	// BIAS

	static const std::string biasComponentId = "bias";

	static const float biasDefaultValue = 0.0f;
	static const float biasMinimumValue = -1.0f;
	static const float biasMaximumValue = 1.0f;
	static const float biasIntervalValue = 0.01f;
	static const juce::NormalisableRange<float> biasNormalizableRange = juce::NormalisableRange<float>(
		biasMinimumValue, 
		biasMaximumValue, 
		biasIntervalValue);

	// GAIN

	static const std::string gainComponentId = "gain";

	static constexpr float gainMinimumValue = -128.0f;
	static constexpr float gainMaximumValue = 128.0f;
	static constexpr float gainIntervalValue = 0.01f;
	static constexpr float gainDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> gainNormalizableRange = juce::NormalisableRange<float>(
		gainMinimumValue, 
		gainMaximumValue, 
		gainIntervalValue);

	// GAIN

	static const std::string dryWetComponentId = "gain";

	static constexpr float dryWetMinimumValue = 0.0f;
	static constexpr float dryWetMaximumValue = 1.0f;
	static constexpr float dryWetIntervalValue = 0.001f;
	static constexpr float notWetDefaultValue = 0.0f;
	static constexpr float allDryDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> dryWetNormalizableRange = juce::NormalisableRange<float>(
		dryWetMinimumValue, 
		dryWetMaximumValue, 
		dryWetIntervalValue);


	// COMPRESSION

	static const std::string noiseGateComponentId = "noise_gate";

	static const std::string compressorThresholdComponentId = "comp_threshold";
	static const std::string compressorAttackComponentId = "comp_attack";
	static const std::string compressorRatioComponentId = "comp_ratio";
	static const std::string compressorReleaseComponentId = "comp_release";

	static const std::string thresholdComponentId = "threshold";
	static constexpr float thresholdMinimumValue = -128.0f;
	static constexpr float thresholdMaximumValue = 0.0f;
	static constexpr float thresholdIntervalValue = 0.01f;
	static constexpr float thresholdDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> thresholdNormalizableRange = juce::NormalisableRange<float>(
		thresholdMinimumValue, 
		thresholdMaximumValue, 
		thresholdIntervalValue);

	static const std::string ratioComponentId = "ratio";
	static constexpr float ratioMinimumValue = 1.0f;
	static constexpr float ratioMaximumValue = 100.0f;
	static constexpr float ratioIntervalValue = 1.0f;
	static constexpr float ratioDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> ratioNormalizableRange = juce::NormalisableRange<float>(
		ratioMinimumValue,
		ratioMaximumValue,
		ratioIntervalValue);

	static const std::string attackComponentId = "attack";
	static constexpr float attackMinimumValue = 0.0f;
	static constexpr float attackMaximumValue = 1000.0f;
	static constexpr float attackIntervalValue = 1.0f;
	static constexpr float attackDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> attackNormalizableRange = juce::NormalisableRange<float>(
		attackMinimumValue, 
		attackMaximumValue, 
		attackIntervalValue);

	static const std::string releaseComponentId = "release";
	static constexpr float releaseMinimumValue = 0.0f;
	static constexpr float releaseMaximumValue = 10000.0f;
	static constexpr float releaseIntervalValue = 1.0f;
	static constexpr float releaseDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> releaseNormalizableRange = juce::NormalisableRange<float>(
		releaseMinimumValue, 
		releaseMaximumValue, 
		releaseIntervalValue);

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

	static const std::string qualityComponentId = "q";
	static constexpr float qualityDefaultValue = 1.0f;
	static constexpr float qualityMinimumValue = 0.001f;
	static constexpr float qualityMaximumValue = 10.f;
	static constexpr float qualityIntervalValue = 0.001f;
	static const juce::NormalisableRange<float> qualityNormalizableRange = juce::NormalisableRange<float>(
		qualityMinimumValue, 
		qualityMaximumValue, 
		qualityIntervalValue);

	static const std::string frequencyComponentId = "frequency";
	static constexpr float frequencyMinimumValue = 15.0f;
	static constexpr float frequencyMaximumValue = 20000.0f;
	static constexpr float frequencyIntervalValue = 1.0f;
	static const juce::NormalisableRange<float> frequencyNormalizableRange = juce::NormalisableRange<float>(
		frequencyMinimumValue, 
		frequencyMaximumValue, 
		frequencyIntervalValue);

	// gain id from above
	static constexpr float eqGainMinimumValue = -128.0f;
	static constexpr float eqGainMaximumValue = 24.0f;
	static constexpr float eqGainInterval = 0.0001;
	static constexpr float eqGainDefaultValue = 1.0f;
	
	static const juce::NormalisableRange<float> eqGainNormalizableRange = juce::NormalisableRange<float>(
		eqGainMinimumValue, 
		eqGainMaximumValue, 
		eqGainInterval);

	static constexpr float peakFilterGainMinimumValue = 0.0001f;
	static const juce::NormalisableRange<float> peakGainNormalizableRange = juce::NormalisableRange<float>(
		peakFilterGainMinimumValue,
		eqGainMaximumValue,
		eqGainInterval);

	// Limiter 

	static constexpr float limiterThresholdMinimumValue = -128.0f;
	static constexpr float limiterThresholdMaximumValue = 0.0f;
	static constexpr float limiterThresholdInterval = 0.01;
	static constexpr float limiterThresholdDefaultValue = -1.0f;
	static const juce::NormalisableRange<float> limiterThresholdNormalizableRange = juce::NormalisableRange<float>(
		limiterThresholdMinimumValue, 
		limiterThresholdMaximumValue, 
		limiterThresholdInterval);

	static constexpr float limiterReleaseMinimumValue = 0.0f;
	static constexpr float limiterReleaseMaximumValue = 2000.0f;
	static constexpr float limiterReleaseInterval = 0.01;
	static constexpr float limiterReleaseDefaultValue = 100.0f;
	static const juce::NormalisableRange<float> limiterReleaseNormalizableRange = juce::NormalisableRange<float>(
		limiterReleaseMinimumValue,
		limiterReleaseMaximumValue,
		limiterReleaseInterval);

	// DELAY

	static constexpr float delayTimeMinimumValue = 0.0f;
	static constexpr float delayTimeMaximumValue = 96000.0f;
	static constexpr float delayTimeInterval = 1.0f;
	static constexpr float delayTimeDefaultValue = 10000.0f;
	static const juce::NormalisableRange<float> delayTimeNormalizableRange = juce::NormalisableRange<float>(
		delayTimeMinimumValue,
		delayTimeMaximumValue,
		delayTimeInterval);

	static constexpr float delayFeedbackMinimumValue = 0.0f;
	static constexpr float delayFeedbackMaximumValue = 1.0f;
	static constexpr float delayFeedbackInterval = 0.001f;
	static constexpr float delayFeedbackDefaultValue = 0.25f;
	static const juce::NormalisableRange<float> delayFeedbackNormalizableRange = juce::NormalisableRange<float>(
		delayFeedbackMinimumValue,
		delayFeedbackMaximumValue,
		delayFeedbackInterval);

	// CHORUS

	static constexpr float chorusRateMinimumValue = 0.0f;
	static constexpr float chorusRateMaximumValue = 99.99f;
	static constexpr float chorusRateInterval = 0.01;
	static constexpr float chorusRateDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> chorusRateNormalizableRange = juce::NormalisableRange<float>(
		chorusRateMinimumValue,
		chorusRateMaximumValue,
		chorusRateInterval);

	static constexpr float chorusDepthMinimumValue = 0.0f;
	static constexpr float chorusDepthMaximumValue = 1.0f;
	static constexpr float chorusDepthInterval = 0.001;
	static constexpr float chorusDepthDefaultValue = 0.1f;
	static const juce::NormalisableRange<float> chorusDepthNormalizableRange = juce::NormalisableRange<float>(
		chorusDepthMinimumValue,
		chorusDepthMaximumValue,
		chorusDepthInterval);

	static constexpr float chorusCenterDelayMinimumValue = 0.0f;
	static constexpr float chorusCenterDelayMaximumValue = 30.0f;
	static constexpr float chorusCenterDelayInterval = 0.01;
	static constexpr float chorusCenterDelayDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> chorusCenterDelayNormalizableRange = juce::NormalisableRange<float>(
		chorusCenterDelayMinimumValue,
		chorusCenterDelayMaximumValue,
		chorusCenterDelayInterval);

	static constexpr float chorusFeedbackMinimumValue = 0.0f;
	static constexpr float chorusFeedbackMaximumValue = 1.0f;
	static constexpr float chorusFeedbackInterval = 0.01;
	static constexpr float chorusFeedbackDefaultValue = 0.75f;
	static const juce::NormalisableRange<float> chorusFeedbackNormalizableRange = juce::NormalisableRange<float>(
		chorusFeedbackMinimumValue,
		chorusFeedbackMaximumValue,
		chorusFeedbackInterval);

	// PHASER

	static constexpr float phaserRateMinimumValue = 0.0f;
	static constexpr float phaserRateMaximumValue = 99.99f;
	static constexpr float phaserRateInterval = 0.01;
	static constexpr float phaserRateDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> phaserRateNormalizableRange = juce::NormalisableRange<float>(
		phaserRateMinimumValue,
		phaserRateMaximumValue,
		phaserRateInterval);

	static constexpr float phaserDepthMinimumValue = 0.0f;
	static constexpr float phaserDepthMaximumValue = 1.0f;
	static constexpr float phaserDepthInterval = 0.001;
	static constexpr float phaserDepthDefaultValue = 0.1f;
	static const juce::NormalisableRange<float> phaserDepthNormalizableRange = juce::NormalisableRange<float>(
		phaserDepthMinimumValue,
		phaserDepthMaximumValue,
		phaserDepthInterval);

	static constexpr float phaserCenterFrequencyMinimumValue = 15.0f;
	static constexpr float phaserCenterFrequencyMaximumValue = 20000.0f;
	static constexpr float phaserCenterFrequencyInterval = 0.1;
	static constexpr float phaserCenterFrequencyDefaultValue = 1000.0f;
	static const juce::NormalisableRange<float> phaserCenterFrequencyNormalizableRange = juce::NormalisableRange<float>(
		phaserCenterFrequencyMinimumValue,
		phaserCenterFrequencyMaximumValue,
		phaserCenterFrequencyInterval);

	static constexpr float phaserFeedbackMinimumValue = 0.0f;
	static constexpr float phaserFeedbackMaximumValue = 1.0f;
	static constexpr float phaserFeedbackInterval = 0.01;
	static constexpr float phaserFeedbackDefaultValue = 0.75f;
	static const juce::NormalisableRange<float> phaserFeedbackNormalizableRange = juce::NormalisableRange<float>(
		phaserFeedbackMinimumValue,
		phaserFeedbackMaximumValue,
		phaserFeedbackInterval);

	// REVERB

	static constexpr float reverbRoomSizeMinimumValue = 0.0f;
	static constexpr float reverbRoomSizeMaximumValue = 1.0f;
	static constexpr float reverbRoomSizeInterval = 0.001;
	static constexpr float reverbRoomSizeDefaultValue = 0.5f;
	static const juce::NormalisableRange<float> reverbRoomSizeNormalizableRange = juce::NormalisableRange<float>(
		reverbRoomSizeMinimumValue,
		reverbRoomSizeMaximumValue,
		reverbRoomSizeInterval);

	static constexpr float reverbDampingMinimumValue = 0.0f;
	static constexpr float reverbDampingMaximumValue = 1.0f;
	static constexpr float reverbDampingInterval = 0.001;
	static constexpr float reverbDampingDefaultValue = 0.5f;
	static const juce::NormalisableRange<float> reverbDampingNormalizableRange = juce::NormalisableRange<float>(
		reverbDampingMinimumValue,
		reverbDampingMaximumValue,
		reverbDampingInterval);

	static constexpr float reverbWidthMinimumValue = 0.0f;
	static constexpr float reverbWidthMaximumValue = 1.0f;
	static constexpr float reverbWidthInterval = 0.001;
	static constexpr float reverbWidthDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> reverbWidthNormalizableRange = juce::NormalisableRange<float>(
		reverbWidthMinimumValue,
		reverbWidthMaximumValue,
		reverbWidthInterval);

	enum class ParameterEnum {		
		NOISE_GATE_ON,
		NOISE_GATE_THRESHOLD,
		NOISE_GATE_ATTACK,
		NOISE_GATE_RATIO,
		NOISE_GATE_RELEASE,

		PRE_COMPRESSOR_THRESHOLD,
		PRE_COMPRESSOR_ATTACK,
		PRE_COMPRESSOR_RATIO,
		PRE_COMPRESSOR_RELEASE,

		MODE,

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
		
		HIGH_PASS_ON,
		HIGH_PASS_FREQUENCY,
		HIGH_PASS_Q,
		
		MID_PEAK_ON,
		MID_PEAK_FREQUENCY,
		MID_PEAK_Q,
		MID_PEAK_GAIN,
		
		HIGH_SHELF_ON,
		HIGH_SHELF_FREQUENCY,
		HIGH_SHELF_Q,
		HIGH_SHELF_GAIN,

		LOW_PASS_ON,
		LOW_PASS_FREQUENCY,
		LOW_PASS_Q,
		
		DELAY_TIME_SAMPLES,
		DELAY_FEEDBACK,
		DELAY_DRY_WET,

		CHORUS_RATE,
		CHORUS_DEPTH,
		CHORUS_CENTER_DELAY,
		CHORUS_FEEDBACK,
		CHORUS_MIX,
		
		PHASER_RATE,
		PHASER_DEPTH,
		PHASER_CENTER_FREQUENCY,
		PHASER_FEEDBACK,
		PHASER_MIX,
		
		REVERB_ROOM_SIZE,
		REVERB_DAMPING,
		REVERB_WET_LEVEL,
		REVERB_DRY_LEVEL,
		REVERB_WIDTH,

		CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON,
		OUTPUT_GAIN,
		LIMITER_THRESHOLD,
		LIMITER_RELEASE
	};

	static const std::string noiseGateOnId = "noise_gate_on";
	static const std::string noiseGateThresholdId = "noise_gate_threshold";
	static const std::string noiseGateAttackId = "noise_gate_attack";
	static const std::string noiseGateRatioId = "noise_gate_ratio";
	static const std::string noiseGateReleaseId = "noise_gate_release";
	static const std::string noiseGateGainId = "noise_gate_gain";

	static const std::string preCompressorThresholdId = "pre_comp_thresh";
	static const std::string preCompressorAttackId = "pre_comp_attack";
	static const std::string preCompressorRatioId = "pre_comp_ratio";
	static const std::string preCompressorReleaseId = "pre_comp_release";

	static const std::string modeId = "mode";

	static const std::string stage1OnId = "stage_1_on";
	static const std::string stage1InputGainId = "stage_1_input_gain";
	static const std::string stage1WaveShaperId = "stage_1_wave_shaper";
	static const std::string stage1OutputGainId = "stage_1_output_gain";
	static const std::string stage1DryWetId = "stage_1_dry_wet";

	static const std::string stage2OnId = "stage_2_on";
	static const std::string stage2InputGainId = "stage_2_input_gain";
	static const std::string stage2WaveShaperId = "stage_2_wave_shaper";
	static const std::string stage2OutputGainId = "stage_2_output_gain";
	static const std::string stage2DryWetId = "stage_2_dry_wet";

	static const std::string stage3OnId = "stage_3_on";
	static const std::string stage3InputGainId = "stage_3_input_gain";
	static const std::string stage3WaveShaperId = "stage_3_wave_shaper";
	static const std::string stage3OutputGainId = "stage_3_output_gain";
	static const std::string stage3DryWetId = "stage_3_dry_wet";

	static const std::string stage4OnId = "stage_4_on";
	static const std::string stage4InputGainId = "stage_4_input_gain";
	static const std::string stage4WaveShaperId = "stage_4_wave_shaper";
	static const std::string stage4OutputGainId = "stage_4_output_gain";
	static const std::string stage4DryWetId = "stage_4_dry_wet";

	static const std::string postCompressorThresholdId = "post_comp_thresh";
	static const std::string postCompressorAttackId = "post_comp_attack";
	static const std::string postCompressorRatioId = "post_comp_ratio";
	static const std::string postCompressorReleaseId = "post_comp_release";
	static const std::string postCompressorGainId = "post_comp_gain";

	static const std::string highPassOnId = "high-pass_on";
	static const std::string highPassFrequencyId = "high-pass_frequency";
	static const std::string highPassQId = "high-pass_q";

	static const std::string midPeakOnId = "mid-peak_on";
	static const std::string midPeakFrequencyId = "mid-peak_frequency";
	static const std::string midPeakQId = "mid-peak_q";
	static const std::string midPeakGainId = "mid-peak_gain";

	static const std::string highShelfOnId = "high-shelf_on";
	static const std::string highShelfFrequencyId = "high-shelf_frequency";
	static const std::string highShelfQId = "high-shelf_q";
	static const std::string highShelfGainId = "high-shelf_gain";

	static const std::string lowPassOnId = "low-pass_on";
	static const std::string lowPassFrequencyId = "low-pass_frequency";
	static const std::string lowPassQId = "low-pass_q";

	static const std::string delayTimeId = "delay_time";
	static const std::string delayFeedbackId = "delay_feedback";
	static const std::string delayDryWetId = "delay_dry_wet";

	static const std::string chorusRateId = "chorus_rate";
	static const std::string chorusDepthId = "chorus_depth";
	static const std::string chorusCenterDelayId = "chorus_center_delay";
	static const std::string chorusFeedbackId = "chorus_feedback";
	static const std::string chorusMixId = "chorus_mix";

	static const std::string phaserRateId = "phaser_rate";
	static const std::string phaserDepthId = "phaser_depth";
	static const std::string phaserCenterFrequencyId = "phaser_center_freq";
	static const std::string phaserFeedbackId = "phaser_feedback";
	static const std::string phaserMixId = "phaser_mix";

	static const std::string reverbRoomSize = "reverb_room_size";
	static const std::string reverbDamping = "reverb_damping";
	static const std::string reverbWetLevel = "reverb_wet_level";
	static const std::string reverbDryLevel = "reverb_dry_level";
	static const std::string reverbWidth = "reverb_width";

	static const std::string cabinetImpulseResponseConvolutionOnId = "cab_on";
	static const std::string outputGainId = "output_gain";

	static const std::string limiterThresholdId = "limiter_threshold";
	static const std::string limiterReleaseId = "limiter_release";

	static const std::map<std::string, ParameterEnum> parameterIdToEnumMap {
		{noiseGateOnId, ParameterEnum::NOISE_GATE_ON},
		{noiseGateThresholdId, ParameterEnum::NOISE_GATE_THRESHOLD},
		{noiseGateAttackId, ParameterEnum::NOISE_GATE_ATTACK},
		{noiseGateRatioId, ParameterEnum::NOISE_GATE_RATIO},
		{noiseGateReleaseId, ParameterEnum::NOISE_GATE_RELEASE},
		
		{preCompressorThresholdId, ParameterEnum::PRE_COMPRESSOR_THRESHOLD},
		{preCompressorAttackId, ParameterEnum::PRE_COMPRESSOR_ATTACK},
		{preCompressorRatioId, ParameterEnum::PRE_COMPRESSOR_RATIO},
		{preCompressorReleaseId, ParameterEnum::PRE_COMPRESSOR_RELEASE},

		{modeId, ParameterEnum::MODE},
		
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
		
		{biasComponentId, ParameterEnum::BIAS},
		
		{postCompressorThresholdId, ParameterEnum::POST_COMPRESSOR_THRESHOLD},
		{postCompressorAttackId, ParameterEnum::POST_COMPRESSOR_ATTACK},
		{postCompressorRatioId, ParameterEnum::POST_COMPRESSOR_RATIO},
		{postCompressorReleaseId, ParameterEnum::POST_COMPRESSOR_RELEASE},
		{postCompressorGainId, ParameterEnum::POST_COMPRESSOR_GAIN},
		
		{highPassOnId, ParameterEnum::HIGH_PASS_ON},
		{highPassFrequencyId, ParameterEnum::HIGH_PASS_FREQUENCY},
		{highPassQId, ParameterEnum::HIGH_PASS_Q},
		
		{midPeakOnId, ParameterEnum::MID_PEAK_ON},
		{midPeakFrequencyId, ParameterEnum::MID_PEAK_FREQUENCY},
		{midPeakQId, ParameterEnum::MID_PEAK_Q},
		{midPeakGainId, ParameterEnum::MID_PEAK_GAIN},
		
		{highShelfOnId, ParameterEnum::HIGH_SHELF_ON},
		{highShelfFrequencyId, ParameterEnum::HIGH_SHELF_FREQUENCY},
		{highShelfQId, ParameterEnum::HIGH_SHELF_Q},
		{highShelfGainId, ParameterEnum::HIGH_SHELF_GAIN},

		{lowPassOnId, ParameterEnum::LOW_PASS_ON},
		{lowPassFrequencyId, ParameterEnum::LOW_PASS_FREQUENCY},
		{lowPassQId, ParameterEnum::LOW_PASS_Q},
		
		{cabinetImpulseResponseConvolutionOnId, ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON},
		
		{outputGainId, ParameterEnum::OUTPUT_GAIN},
		
		{limiterThresholdId, ParameterEnum::LIMITER_THRESHOLD},
		{limiterReleaseId, ParameterEnum::LIMITER_RELEASE},

		{delayTimeId, ParameterEnum::DELAY_TIME_SAMPLES},
		{delayFeedbackId, ParameterEnum::DELAY_FEEDBACK},
		{delayDryWetId, ParameterEnum::DELAY_DRY_WET},

		{chorusRateId, ParameterEnum::CHORUS_RATE},
		{chorusDepthId, ParameterEnum::CHORUS_DEPTH},
		{chorusCenterDelayId, ParameterEnum::CHORUS_CENTER_DELAY},
		{chorusFeedbackId, ParameterEnum::CHORUS_FEEDBACK},
		{chorusMixId, ParameterEnum::CHORUS_MIX},

		{phaserRateId, ParameterEnum::PHASER_RATE},
		{phaserDepthId, ParameterEnum::PHASER_DEPTH},
		{phaserCenterFrequencyId, ParameterEnum::PHASER_CENTER_FREQUENCY},
		{phaserFeedbackId, ParameterEnum::PHASER_FEEDBACK},
		{phaserMixId, ParameterEnum::PHASER_MIX},

		{reverbRoomSize, ParameterEnum::REVERB_ROOM_SIZE},
		{reverbDamping, ParameterEnum::REVERB_DAMPING},
		{reverbWetLevel, ParameterEnum::REVERB_WET_LEVEL},
		{reverbDryLevel, ParameterEnum::REVERB_DRY_LEVEL},
		{reverbWidth, ParameterEnum::REVERB_WIDTH},
	};
}