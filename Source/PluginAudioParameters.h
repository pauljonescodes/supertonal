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

	//

	static const std::string waveshaperComponentId = "wave_shaper";

	static const std::string hyperbolicTangentWaveshaperId = "htan";
	static const std::string softClipWaveshaperId = "soft";
	static const std::string arctangentWaveshaperId = "atan";
	static const std::string cubicNonLineartyWaveshaperId = "cube";
	static const std::string hardClipWaveshaperId = "hard";

	static const std::vector<std::string> waveShaperIds = {
		hyperbolicTangentWaveshaperId,
		softClipWaveshaperId,
		arctangentWaveshaperId,
		cubicNonLineartyWaveshaperId,
		hardClipWaveshaperId,
	};

	static const std::map<std::string, float (*) (float)> waveshaperFunctions = {
		{hyperbolicTangentWaveshaperId, [](float x) { return std::tanh(x); }},
		{softClipWaveshaperId, [](float x) { return x / (std::abs(x) + 1.0f); }},
		{arctangentWaveshaperId, [](float x) { return std::atan(x); }},
		{cubicNonLineartyWaveshaperId, [](float x) { return x - (1.0f / 3.0f) * std::pow(x, 3.0f); }},
		{hardClipWaveshaperId, [](float x) { return std::max(-1.0f, std::min(1.0f, x)); }}
	};

	static const std::string modeComponentId = "mode";

	static const std::string seriesModeId = "series";
	static const std::string parallelModeId = "parallel";

	static const std::vector<std::string> modeIds = {
		parallelModeId,
		seriesModeId,
	};


	static const std::string stage1OnId = "stage_1_on";
	static const std::string stage1InputGainId = "stage_1_input_gain";
	static const std::string stage1WaveShaperId = "stage_1_wave_shaper";
	static const std::string stage1OutputGainId = "stage_1_output_gain";
	static const std::string stage1DryWetId = "stage_1_dry_wet";
	static const std::string stage1ModeId = "stage_1_mode";

	static const std::string stage2OnId = "stage_2_on";
	static const std::string stage2InputGainId = "stage_2_input_gain";
	static const std::string stage2WaveShaperId = "stage_2_wave_shaper";
	static const std::string stage2OutputGainId = "stage_2_output_gain";
	static const std::string stage2DryWetId = "stage_2_dry_wet";
	static const std::string stage2ModeId = "stage_2_mode";

	static const std::string stage3OnId = "stage_3_on";
	static const std::string stage3InputGainId = "stage_3_input_gain";
	static const std::string stage3WaveShaperId = "stage_3_wave_shaper";
	static const std::string stage3OutputGainId = "stage_3_output_gain";
	static const std::string stage3DryWetId = "stage_3_dry_wet";
	static const std::string stage3ModeId = "stage_3_mode";

	static const std::string cabinetImpulseResponseConvolutionOnId = "cab_on";
	static const std::string outputGainId = "cab_gain";

	static const std::string ampCompThresholdId = "amp_comp_threshold";
	static const std::string ampCompAttackId = "amp_comp_attack";
	static const std::string ampCompRatioId = "amp_comp_ratio";
	static const std::string ampCompReleaseId = "amp_comp_release";
	static const std::string ampCompGainId = "amp_comp_gain";

	static const std::string ampHighPassOnId = "amp_high-pass_on";
	static const std::string ampHighPassFrequencyId = "amp_high-pass_frequency";
	static const std::string ampHighPassQId = "amp_high-pass_q";

	static const std::string ampMidPeakOnId = "amp_mid-peak_on";
	static const std::string ampMidPeakFrequencyId = "amp_mid-peak_frequency";
	static const std::string ampMidPeakQId = "amp_mid-peak_q";
	static const std::string ampMidPeakGainId = "amp_mid-peak_gain";

	static const std::string ampHighShelfOnId = "amp_high-shelf_on";
	static const std::string ampHighShelfFrequencyId = "amp_high-shelf_frequency";
	static const std::string ampHighShelfQId = "amp_high-shelf_q";
	static const std::string ampHighShelfGainId = "amp_high-shelf_gain";

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
		STAGE1_ON,
		STAGE1_INPUT_GAIN,
		STAGE1_WAVE_SHAPER,
		STAGE1_OUTPUT_GAIN,
		STAGE1_DRY_WET,
		STAGE1_MODE,
		STAGE2_ON,
		STAGE2_INPUT_GAIN,
		STAGE2_WAVE_SHAPER,
		STAGE2_OUTPUT_GAIN,
		STAGE2_DRY_WET,
		STAGE2_MODE,
		STAGE3_ON,
		STAGE3_INPUT_GAIN,
		STAGE3_WAVE_SHAPER,
		STAGE3_OUTPUT_GAIN,
		STAGE3_DRY_WET,
		STAGE3_MODE,
		BIAS,
		AMP_COMPRESSOR_THRESHOLD,
		AMP_COMPRESSOR_ATTACK,
		AMP_COMPRESSOR_RATIO,
		AMP_COMPRESSOR_RELEASE,
		AMP_COMPRESSOR_GAIN,
		AMP_HIGH_PASS_ON,
		AMP_HIGH_PASS_FREQUENCY,
		AMP_HIGH_PASS_Q,
		AMP_MID_PEAK_ON,
		AMP_MID_PEAK_FREQUENCY,
		AMP_MID_PEAK_Q,
		AMP_MID_PEAK_GAIN,
		AMP_HIGH_SHELF_ON,
		AMP_HIGH_SHELF_FREQUENCY,
		AMP_HIGH_SHELF_Q,
		AMP_HIGH_SHELF_GAIN,
		CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON,
		OUTPUT_GAIN
	};

	static const std::map<std::string, ParameterEnum> parameterIdToEnumMap{
		{stage1OnId, ParameterEnum::STAGE1_ON},
		{stage1InputGainId, ParameterEnum::STAGE1_INPUT_GAIN},
		{stage1WaveShaperId, ParameterEnum::STAGE1_WAVE_SHAPER},
		{stage1OutputGainId, ParameterEnum::STAGE1_OUTPUT_GAIN},
		{stage1DryWetId, ParameterEnum::STAGE1_DRY_WET},
		{stage1ModeId, ParameterEnum::STAGE1_MODE},
		{stage2OnId, ParameterEnum::STAGE2_ON},
		{stage2InputGainId, ParameterEnum::STAGE2_INPUT_GAIN},
		{stage2WaveShaperId, ParameterEnum::STAGE2_WAVE_SHAPER},
		{stage2OutputGainId, ParameterEnum::STAGE2_OUTPUT_GAIN},
		{stage2DryWetId, ParameterEnum::STAGE2_DRY_WET},
		{stage2ModeId, ParameterEnum::STAGE2_MODE},
		{stage3OnId, ParameterEnum::STAGE3_ON},
		{stage3InputGainId, ParameterEnum::STAGE3_INPUT_GAIN},
		{stage3WaveShaperId, ParameterEnum::STAGE3_WAVE_SHAPER},
		{stage3OutputGainId, ParameterEnum::STAGE3_OUTPUT_GAIN},
		{stage3DryWetId, ParameterEnum::STAGE3_DRY_WET},
		{stage3ModeId, ParameterEnum::STAGE3_MODE},
		{biasComponentId, ParameterEnum::BIAS},
		{ampCompThresholdId, ParameterEnum::AMP_COMPRESSOR_THRESHOLD},
		{ampCompAttackId, ParameterEnum::AMP_COMPRESSOR_ATTACK},
		{ampCompRatioId, ParameterEnum::AMP_COMPRESSOR_RATIO},
		{ampCompReleaseId, ParameterEnum::AMP_COMPRESSOR_RELEASE},
		{ampCompGainId, ParameterEnum::AMP_COMPRESSOR_GAIN},
		{ampHighPassOnId, ParameterEnum::AMP_HIGH_PASS_ON},
		{ampHighPassFrequencyId, ParameterEnum::AMP_HIGH_PASS_FREQUENCY},
		{ampHighPassQId, ParameterEnum::AMP_HIGH_PASS_Q},
		{ampMidPeakOnId, ParameterEnum::AMP_MID_PEAK_ON},
		{ampMidPeakFrequencyId, ParameterEnum::AMP_MID_PEAK_FREQUENCY},
		{ampMidPeakQId, ParameterEnum::AMP_MID_PEAK_Q},
		{ampMidPeakGainId, ParameterEnum::AMP_MID_PEAK_GAIN},
		{ampHighShelfOnId, ParameterEnum::AMP_HIGH_SHELF_ON},
		{ampHighShelfFrequencyId, ParameterEnum::AMP_HIGH_SHELF_FREQUENCY},
		{ampHighShelfQId, ParameterEnum::AMP_HIGH_SHELF_Q},
		{ampHighShelfGainId, ParameterEnum::AMP_HIGH_SHELF_GAIN},
		{cabinetImpulseResponseConvolutionOnId, ParameterEnum::CABINET_IMPULSE_RESPONSE_CONVOLUTION_ON},
		{outputGainId, ParameterEnum::OUTPUT_GAIN}
	};
}