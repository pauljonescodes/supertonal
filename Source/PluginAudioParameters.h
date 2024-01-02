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

	// Stages

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

	static const std::string compressorThresholdId = "comp_threshold";
	static const std::string compressorAttackId = "comp_attack";
	static const std::string compressorRatioId = "comp_ratio";
	static const std::string compressorReleaseId = "comp_release";
	static const std::string compressorGainId = "comp_gain";

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

	// Cab

	static const std::string cabinetImpulseResponseConvolutionOnId = "cab_on";
	static const std::string outputGainId = "output_gain";

	// BYPASS

	static const std::string onComponentId = "on";

	// BIAS

	static const std::string biasComponentId = "bias";

	static const float biasDefaultValue = 0.0f;
	static const float biasMinimumValue = -1.0f;
	static const float biasMaximumValue = 1.0f;
	static const float biasIntervalValue = 0.01f;
	static const juce::NormalisableRange<float> biasNormalizableRange = juce::NormalisableRange<float>(biasMinimumValue, biasMaximumValue, biasIntervalValue);

	// GAIN

	static const std::string gainComponentId = "gain";

	static constexpr float gainMinimumValue = -128.0f;
	static constexpr float gainMaximumValue = 128.0f;
	static constexpr float gainIntervalValue = 0.01f;
	static constexpr float gainDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> gainNormalizableRange = juce::NormalisableRange<float>(gainMinimumValue, gainMaximumValue, gainIntervalValue);

	// GAIN

	static const std::string dryWetComponentId = "gain";

	static constexpr float dryWetMinimumValue = 0.0f;
	static constexpr float dryWetMaximumValue = 1.0f;
	static constexpr float dryWetIntervalValue = 0.001f;
	static constexpr float dryWetDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> dryWetNormalizableRange = juce::NormalisableRange<float>(dryWetMinimumValue, dryWetMaximumValue, dryWetIntervalValue);


	// COMPRESSION

	static const std::string compressionComponentId = "comp";

	static const std::string thresholdComponentId = "threshold";
	static constexpr float thresholdMinimumValue = -64.0f;
	static constexpr float thresholdMaximumValue = 0.0f;
	static constexpr float thresholdIntervalValue = 0.01f;
	static constexpr float thresholdDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> thresholdNormalizableRange = juce::NormalisableRange<float>(thresholdMinimumValue, thresholdMaximumValue, thresholdIntervalValue);

	static const std::string attackComponentId = "attack";
	static constexpr float attackMinimumValue = 0.0f;
	static constexpr float attackMaximumValue = 1000.0f;
	static constexpr float attackIntervalValue = 1.0f;
	static constexpr float attackDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> attackNormalizableRange = juce::NormalisableRange<float>(attackMinimumValue, attackMaximumValue, attackIntervalValue);

	static const std::string ratioComponentId = "ratio";
	static constexpr float ratioMinimumValue = 1.0f;
	static constexpr float ratioMaximumValue = 100.0f;
	static constexpr float ratioIntervalValue = 1.0f;
	static constexpr float ratioDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> ratioNormalizableRange = juce::NormalisableRange<float>(ratioMinimumValue, ratioMaximumValue, ratioIntervalValue);

	static const std::string releaseComponentId = "release";
	static constexpr float releaseMinimumValue = 0.0f;
	static constexpr float releaseMaximumValue = 1000.0f;
	static constexpr float releaseIntervalValue = 1.0f;
	static constexpr float releaseDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> releaseNormalizableRange = juce::NormalisableRange<float>(releaseMinimumValue, releaseMaximumValue, releaseIntervalValue);

	// EQ

	static const std::string highPassEqualizationComponentId = "high-pass";
	static const std::string midPeakEqualizationComponentId = "mid-peak";
	static const std::string highShelfEqualizationComponentId = "high-shelf";

	static constexpr float lowShelfFrequencyDefaultValue = 20.0f;
	static constexpr float midPeakFrequencyDefaultValue = 1000.0f;
	static constexpr float highShelfFrequencyDefaultValue = 15000.0f;

	static const std::map<std::string, float> equalizationTypeIdToDefaultFrequencyMap = {
	{highShelfEqualizationComponentId,highShelfFrequencyDefaultValue},
	{midPeakEqualizationComponentId,midPeakFrequencyDefaultValue},
	{highPassEqualizationComponentId,lowShelfFrequencyDefaultValue}
	};

	static const std::string qualityComponentId = "q";
	static constexpr float qualityDefaultValue = 1.1f;
	static constexpr float qualityMinimumValue = 0.1f;
	static constexpr float qualityMaximumValue = 10.f;
	static constexpr float qualityIntervalValue = 0.001f;
	static const juce::NormalisableRange<float> qualityNormalizableRange = juce::NormalisableRange<float>(qualityMinimumValue, qualityMaximumValue, qualityIntervalValue);

	static const std::string frequencyComponentId = "frequency";
	static constexpr float frequencyMinimumValue = 15.0f;
	static constexpr float frequencyMaximumValue = 20000.0f;
	static constexpr float frequencyIntervalValue = 1.0f;
	static const juce::NormalisableRange<float> frequencyNormalizableRange = juce::NormalisableRange<float>(frequencyMinimumValue, frequencyMaximumValue, frequencyIntervalValue);

	// gain id from above
	static constexpr float eqGainMinimumValue = 0.001f;
	static constexpr float eqGainMaximumValue = 12.0f;
	static constexpr float eqGainInterval = 0.001;
	static constexpr float eqGainDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> eqGainNormalizableRange = juce::NormalisableRange<float>(eqGainMinimumValue, eqGainMaximumValue, eqGainInterval);

	//

	static const std::string ampComponentId = "amp";
	static const std::string outputComponentId = "output";


	enum class ParameterEnum {
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
		COMPRESSOR_THRESHOLD,
		COMPRESSOR_ATTACK,
		COMPRESSOR_RATIO,
		COMPRESSOR_RELEASE,
		COMPRESSOR_GAIN,
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
		CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON,
		OUTPUT_GAIN
	};

	static const std::map<std::string, ParameterEnum> parameterIdToEnumMap{
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
		{compressorThresholdId, ParameterEnum::COMPRESSOR_THRESHOLD},
		{compressorAttackId, ParameterEnum::COMPRESSOR_ATTACK},
		{compressorRatioId, ParameterEnum::COMPRESSOR_RATIO},
		{compressorReleaseId, ParameterEnum::COMPRESSOR_RELEASE},
		{compressorGainId, ParameterEnum::COMPRESSOR_GAIN},
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
		{cabinetImpulseResponseConvolutionOnId, ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON},
		{outputGainId, ParameterEnum::OUTPUT_GAIN}
	};
}