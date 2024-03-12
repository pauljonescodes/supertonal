#pragma once

#include "MouseDriveWDF.h"
#include "../../Utilities/CircuitQuantityHelper.h"
#include <JuceHeader.h>

class MouseDrive 
{
public:

    inline static const float sVolumeDefaultValue = -20.0f;

    MouseDrive();

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();
    void setDistortion(float newValue);
    void setVolume(float newValue);
    void setLowPassFrequency(float newValue);

private:
    std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> mDistortionSmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mVolumeSmoothedValue;

    float mCurrentSampleRate = 44100.0f;
    float mCurrentLowPassFrequency = 20000;

    MouseDriveWDF mWaveDesignFilter[2];
    juce::dsp::Gain<float> mGain;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mLowPassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mDirectCurrentBlockerHighPassFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MouseDrive)
};
