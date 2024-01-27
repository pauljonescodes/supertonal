#pragma once

#include <JuceHeader.h>

class ParametricEqualiser
{
public:
    inline static const std::array<float, 7> sFrequencies = { 100, 200, 400, 800, 1600, 3200, 6400 };
    inline static const std::array<float, 6> sQualities = { 2.83, 2.33, 2.45, 2.77, 2.09, 3.46 };

    explicit ParametricEqualiser();

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();

    /* 0-6 are EQ, 7 is output */
    void setGainDbAtIndex(float newGain, int index);

private:

    /* +/- 15 */
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 8> mDecibelGainSmoothedValues;

    /* 100,  200,  400,  800,  1600, 3200, 6400 */
    std::array<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>, 7> mFilters;

    juce::dsp::Gain<float> mLevelGain;

    float mCurrentSampleRate = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricEqualiser)
};
