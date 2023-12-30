/*
  ==============================================================================

	PluginAudioParameters.h
	Created: 29 Dec 2023 3:38:11pm
	Author:  paulm

  ==============================================================================
*/

#pragma once

#include <string>

namespace apvts
{
	static const std::string identifier = "supertonal-apvts";

	// Define each ID as a static const with camel case and "Id" suffix
	
	static const std::string overdriveBiasId = "od_bias";
	
	static const std::string inputGainId = "input_gain";
	static const std::string ampGainId = "amp_gain";
	static const std::string overdriveGainId = "od_gain";
	static const std::string outputGainId = "output_gain";

	static const std::string ampImpulseResponseConvolutionOnId = "amp_on";
	static const std::string cabImpulseResponseConvolutionOnId = "cab_on";

	static const std::string overdriveTanhShaperOnId = "od_tanh-shaper_on";
	static const std::string overdriveClipShaperOnId = "od_clip-shaper_on";
	static const std::string ampLowShelfOnId = "amp_low-shelf_on";
	static const std::string ampMidPeakOnId = "amp_mid-peak_on";
	static const std::string ampHighShelfOnId = "amp_high-shelf_on";

	static const std::string ampCompThresholdId = "amp_comp_threshold";
	static const std::string ampCompAttackId = "amp_comp_attack";
	static const std::string ampCompRatioId = "amp_comp_ratio";
	static const std::string ampCompReleaseId = "amp_comp_release";
	
	static const std::string ampLowShelfFrequencyId = "amp_low-shelf_frequency";
	static const std::string ampLowShelfQId = "amp_low-shelf_q";
	static const std::string ampLowShelfGainId = "amp_low-shelf_gain";
	
	static const std::string ampMidPeakFrequencyId = "amp_mid-peak_frequency";
	static const std::string ampMidPeakQId = "amp_mid-peak_q";
	static const std::string ampMidPeakGainId = "amp_mid-peak_gain";
	
	static const std::string ampHighShelfFrequencyId = "amp_high-shelf_frequency";
	static const std::string ampHighShelfQId = "amp_high-shelf_q";
	static const std::string ampHighShelfGainId = "amp_high-shelf_gain";

	// Create a vector containing all the IDs
	static const std::vector<std::string> ids = {
		inputGainId,
		overdriveTanhShaperOnId,
		overdriveClipShaperOnId,
		overdriveBiasId,
		overdriveGainId,
		ampImpulseResponseConvolutionOnId,
		ampCompThresholdId,
		ampCompAttackId,
		ampCompRatioId,
		ampCompReleaseId,
		ampLowShelfOnId,
		ampLowShelfFrequencyId,
		ampLowShelfQId,
		ampLowShelfGainId,
		ampMidPeakOnId,
		ampMidPeakFrequencyId,
		ampMidPeakQId,
		ampMidPeakGainId,
		ampHighShelfOnId,
		ampHighShelfFrequencyId,
		ampHighShelfQId,
		ampHighShelfGainId,
		ampGainId,
		cabImpulseResponseConvolutionOnId,
		outputGainId
	};

	static const std::string inputId = "input";
	static const std::string overdriveId = "od";
	static const std::string ampId = "amp";
	static const std::string outputId = "output";

	static const std::string tanhWaveShaperId = "tanh-shaper";
	static const std::string softClipWaveShaperId = "clip-shaper";

	// BYPASS

	static const std::string onId = "on";

	// BIAS

	static const std::string biasId = "bias";

	static const float biasDefaultValue = 0.0f;
	static const float biasMinimumValue = -1.0f;
	static const float biasMaximumValue = 1.0f;
	static const float biasIntervalValue = 0.01f;
	static const juce::NormalisableRange<float> biasNormalizableRange = juce::NormalisableRange<float>(biasMinimumValue, biasMaximumValue, biasIntervalValue);

	// GAIN

	static const std::string gainId = "gain";

	static constexpr float gainMinimumValue = -64.0f;
	static constexpr float gainMaximumValue = 64.0f;
	static constexpr float gainIntervalValue = 0.01f;
	static constexpr float gainDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> gainNormalizableRange = juce::NormalisableRange<float>(gainMinimumValue, gainMaximumValue, gainIntervalValue);

	// COMPRESSION

	static const std::string compressionId = "comp";

	static const std::string thresholdId = "threshold";
	static constexpr float thresholdMinimumValue = -64.0f;
	static constexpr float thresholdMaximumValue = 0.0f;
	static constexpr float thresholdIntervalValue = 0.01f;
	static constexpr float thresholdDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> thresholdNormalizableRange = juce::NormalisableRange<float>(thresholdMinimumValue, thresholdMaximumValue, thresholdIntervalValue);

	static const std::string attackId = "attack";
	static constexpr float attackMinimumValue = 0.0f;
	static constexpr float attackMaximumValue = 1000.0f;
	static constexpr float attackIntervalValue = 1.0f;
	static constexpr float attackDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> attackNormalizableRange = juce::NormalisableRange<float>(attackMinimumValue, attackMaximumValue, attackIntervalValue);

	static const std::string ratioId = "ratio";
	static constexpr float ratioMinimumValue = 1.0f;
	static constexpr float ratioMaximumValue = 100.0f;
	static constexpr float ratioIntervalValue = 1.0f;
	static constexpr float ratioDefaultValue = 1.0f;
	static const juce::NormalisableRange<float> ratioNormalizableRange = juce::NormalisableRange<float>(ratioMinimumValue, ratioMaximumValue, ratioIntervalValue);

	static const std::string releaseId = "release";
	static constexpr float releaseMinimumValue = 0.0f;
	static constexpr float releaseMaximumValue = 1000.0f;
	static constexpr float releaseIntervalValue = 1.0f;
	static constexpr float releaseDefaultValue = 0.0f;
	static const juce::NormalisableRange<float> releaseNormalizableRange = juce::NormalisableRange<float>(releaseMinimumValue, releaseMaximumValue, releaseIntervalValue);

	// EQ

	static const std::string lowShelfEqualizationId = "low-shelf";
	static const std::string midPeakEqualizationId = "mid-peak";
	static const std::string highShelfEqualizationId = "high-shelf";

	static constexpr float lowShelfFrequencyDefaultValue = 20.0f;
	static constexpr float midPeakFrequencyDefaultValue = 1000.0f;
	static constexpr float highShelfFrequencyDefaultValue = 15000.0f;

	static const std::map<std::string, float> equalizationTypeIdToDefaultFrequencyMap = {
	{highShelfEqualizationId,highShelfFrequencyDefaultValue},
	{midPeakEqualizationId,midPeakFrequencyDefaultValue},
	{lowShelfEqualizationId,lowShelfFrequencyDefaultValue}
	};

	static const std::string qualityId = "q";
	static constexpr float qualityDefaultValue = 1.1f;
	static constexpr float qualityMinimumValue = 0.1f;
	static constexpr float qualityMaximumValue = 10.f;
	static constexpr float qualityIntervalValue = 0.01f;
	static const juce::NormalisableRange<float> qualityNormalizableRange = juce::NormalisableRange<float>(qualityMinimumValue, qualityMaximumValue, qualityIntervalValue);

	static const std::string frequencyId = "frequency";
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
}