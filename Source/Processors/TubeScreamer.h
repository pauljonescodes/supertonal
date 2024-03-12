#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <JuceHeader.h>
#include "TubeScreamerWDF.h"
#include "CircuitQuantityHelper.h"
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
