/*
  ==============================================================================

	InstrumentEqualiser.h
	Created: 29 Jan 2024 10:37:15pm
	Author:  paulm

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class InstrumentEqualiser
{
public:

	static constexpr float sHighPassFrequencyDefaultValue = 20.0f;
	inline static const juce::NormalisableRange<float> sHighPassFrequencyNormalisableRange = { 20.0f, 2000.0f };

	static constexpr float sLowPeakFrequencyDefaultValue = 120.0f;
	inline static const juce::NormalisableRange<float> sLowPeakFrequencyNormalisableRange = { 20.0f, 500.0f };

	static constexpr float sLowMidPeakFrequencyDefaultValue = 800.0f;
	inline static const juce::NormalisableRange<float> sLowMidPeakFrequencyNormalisableRange = { 125.0f, 4000.0f };

	static constexpr float sHighMidPeakFrequencyDefaultValue = 2400.0f;
	inline static const juce::NormalisableRange<float> sHighMidPeakFrequencyNormalisableRange = { 500.0f, 8000.0f };

	static constexpr float sHighPeakFrequencyDefaultValue = 6400.0f;
	inline static const juce::NormalisableRange<float> sHighPeakFrequencyNormalisableRange = { 2000.0f, 20000.0f };

	static constexpr float sLowPassFrequencyDefaultValue = 20000.0f;
	inline static const juce::NormalisableRange<float> sLowPassFrequencyNormalisableRange = { 6000.0f, 20000.0f };

	inline static const juce::NormalisableRange<float> sQualityNormalisableRange = {
			0.001f,
			10.0f,
	[](float start, float end, float normalised)
	{
		return start + (std::log2(normalised + 1) / std::log2(end - start + 1)) * (end - start);
	},
	[](float start, float end, float unnormalised)
	{
		return (std::exp2((unnormalised - start) / (end - start) * std::log2(end - start + 1)) - 1);
	}
	};
	inline static const juce::NormalisableRange<float> sDecibelsNormalisableRange = { -18.0f, 18.0f };

	explicit InstrumentEqualiser();

	void prepare(juce::dsp::ProcessSpec& spec);
	void processBlock(juce::AudioBuffer<float>& buffer);
	void reset();

	void setOnAtIndex(bool newValue, int index);
	void setFrequencyAtIndex(float newValue, int index);
	void setGainAtIndex(float newValue, int index);
	void setQualityAtIndex(float newValue, int index);

private:
	float mCurrentSampleRate = 44100.0f;

	std::array<bool, 6> mBypasses = { true, true, true, true, true, true };
	std::array<float, 6> mFrequencies = {
		sHighPassFrequencyDefaultValue,
		sLowPeakFrequencyDefaultValue,
		sLowMidPeakFrequencyDefaultValue,
		sHighMidPeakFrequencyDefaultValue,
		sHighPeakFrequencyDefaultValue,
		sLowPassFrequencyDefaultValue
	};
	std::array<float, 6> mDecibelGains = {
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f
	};
	std::array<float, 6> mQualities = {
0.70710678118654752440L,
0.70710678118654752440L,
0.70710678118654752440L,
0.70710678118654752440L,
0.70710678118654752440L,
0.70710678118654752440L
	};
	std::array<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>, 6> mFilters;

	float getDefaultValueForIndex(int index);
	const juce::NormalisableRange<float>& getFrequencyNormalisableRangeForIndex(int index);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentEqualiser)
};
