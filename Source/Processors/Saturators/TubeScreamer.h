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
#include "../../Utilities/CircuitQuantityHelper.h"
#include "TubeScreamerTone.h"

class TubeScreamer
{
public:
    explicit TubeScreamer ();

    inline static const float sLevelDefaultValue = -10.0f;

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();

    void setDrive(float newGain);
    void setLevel(float newLevel);
    void setDiodeType(int newDiodeType); // 0, 1, or 2
    void setDiodeCount(int newDiodeCount);
	void setTone(float tone);

private:
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mDriveGainSmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mLevelGainSmoothedValue;
    
    int mDiodeType = 0;
    int mDiodeCount = 1;

    std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

    TubeScreamerWDF mWdf[2];
	TubeScreamerTone mTone[2];
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mDirectCurrentBlockerHighPassFilter;

    float getDiodeIs(int diodeType);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeScreamer)
};
