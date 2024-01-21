#pragma once

#include "TubeScreamerWDF.h"
#include "CircuitQuantityHelper.h"
#include <JuceHeader.h>

class TubeScreamer
{
public:
    explicit TubeScreamer ();

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(AudioBuffer<float>& buffer);
    void reset();

    void setGain(float newGain);
    void setDiodeType(int newDiodeType); // 0, 1, or 2
    void setDiodeCount(int newDiodeCount);

private:
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mGainSmoothedValue;
    
    int mDiodeType = 0;
    int mDiodeCount = 1;

    std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

    TubeScreamerWDF wdf[2];
    juce::dsp::Gain<float> mGain;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mDirectCurrentBlockerHighPassFilter;

    float getDiodeIs(int diodeType);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeScreamer)
};