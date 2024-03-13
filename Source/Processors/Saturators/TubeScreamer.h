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

#define _USE_MATH_DEFINES
#include <cmath>

#include <JuceHeader.h>
#include "TubeScreamerWDF.h"
#include "TubeScreamerTone.h"
#include "../../Utilities/CircuitQuantityHelper.h"

class TubeScreamer
{
public:
    inline static const float levelDefaultValue = -10.0f;
    static inline const juce::NormalisableRange<float> levelNormalisableRange = {
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

    static constexpr float toneDefaultValue = 1.0f;
    static inline const juce::NormalisableRange<float> toneNormalisableRange = juce::NormalisableRange<float>(
        0.0f,
        1.0f,
        0.01f);

    static constexpr float driveDefaultValue = 0.5f;
    static inline const juce::NormalisableRange<float> driveNormalisableRange = juce::NormalisableRange<float>(
        0.0f,
        1.0f,
        0.01f);

    static constexpr float diodeTypeMinimumValue = 0.0f;
    static constexpr float diodeTypeMaximumValue = 2.0f;
    static constexpr float diodeTypeInterval = 1.0;
    static constexpr float diodeTypeDefaultValue = 0.0f;
    static inline const juce::NormalisableRange<float> diodeTypeNormalisableRange = juce::NormalisableRange<float>(
        diodeTypeMinimumValue,
        diodeTypeMaximumValue,
        diodeTypeInterval);

    static constexpr float diodeCountMinimumValue = 1.0f;
    static constexpr float diodeCountMaximumValue = 3.0f;
    static constexpr float diodeCountInterval = 1.0;
    static constexpr float diodeCountDefaultValue = 2.0f;
    static inline const juce::NormalisableRange<float> diodeCountNormalisableRange = juce::NormalisableRange<float>(
        diodeCountMinimumValue,
        diodeCountMaximumValue,
        diodeCountInterval);

    explicit TubeScreamer ();

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();

    void setDrive(float newGain);
    void setLevel(float newLevel);
    void setTone(float tone);
    void setDiodeType(int newDiodeType); // 0, 1, or 2
    void setDiodeCount(int newDiodeCount);

private:
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mDriveGainSmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mLevelGainSmoothedValue;
    
    int mDiodeType = 0;
    int mDiodeCount = 1;

    std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

    TubeScreamerWDF mWdf[2];
	TubeScreamerTone mTone[2];
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mDCBlockerHPF;

    float getDiodeIs(int diodeType);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeScreamer)
};
