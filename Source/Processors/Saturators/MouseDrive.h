/*
  ==============================================================================

	This code is based on the Jatin Chowdry's "BYOD"

	https://github.com/Chowdhury-DSP/BYOD

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#pragma once

#include "MouseDriveWDF.h"
#include "../../Utilities/CircuitQuantityHelper.h"
#include <JuceHeader.h>

class MouseDrive
{
public:

	static constexpr float distortionDefaultValue = 0.75;
	static inline const juce::NormalisableRange<float> distortionNormalisableRange = juce::NormalisableRange<float>(
		0.00f,
		1.00f,
		0.01f);

	static constexpr float filterDefaultValue = 20000.0f;
	static inline const juce::NormalisableRange<float> filterNormalisableRange = {
			20.0f,
			20000.0f,
			[=](float start, float end, float normalised)
			{
				return start + (std::exp2(normalised * 6.0f) - 1) * (end - start) / (std::exp2(6.0f) - 1);
			},
			[=](float start, float end, float unnormalised)
			{
				return std::log2(((unnormalised - start) / (end - start) * (std::exp2(6.0f) - 1)) + 1) / 6.0f;
			}
	};

	static constexpr float volumeDefaultValue = -20.0f;
	static inline const juce::NormalisableRange<float> volumeNormalisableRange = {
			-64.0f,
			0.0f,
			[=](float min, float max, float normalised) // convertFrom0to1
			{
				return normalised * (max - min) + min;
			},
			[=](float min, float max, float unnormalised) // convertTo0to1
			{
				return (unnormalised - min) / (max - min);
			}
	};

	MouseDrive();

	void prepare(juce::dsp::ProcessSpec& spec);
	void processBlock(juce::AudioBuffer<float>& buffer);
	void reset();
	void setDistortion(float newValue);
	void setVolume(float newValue);
	void setFilter(float newValue);

private:
	std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> mDistortionSmoothedValue;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mVolumeSmoothedValue;

	float mCurrentSampleRate = 44100.0f;
	float mCurrentLowPassFrequency = 20000;

	MouseDriveWDF mWaveDesignFilter[2];
	juce::dsp::Gain<float> mGain;
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mLowPassFilter;
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mDCBlockerHPF;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MouseDrive)
};
