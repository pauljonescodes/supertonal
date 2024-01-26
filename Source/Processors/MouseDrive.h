#pragma once

#include "MouseDriveWDF.h"
#include "CircuitQuantityHelper.h"
#include <JuceHeader.h>

class MouseDrive 

{
public:
    MouseDrive();

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();
    void setDistortion(float targetValue);
    void setVolume(float targetValue);

private:
    std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> mDistortionSmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mVolumeSmoothedValue;

    MouseDriveWDF mWaveDesignFilter[2];
    juce::dsp::Gain<float> mGain;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mDirectCurrentBlockerHighPassFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MouseDrive)
};
