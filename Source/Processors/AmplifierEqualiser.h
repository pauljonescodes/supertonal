#pragma once
#include <JuceHeader.h>

class AmplifierEqualiser
{
public:
    inline static const juce::NormalisableRange<float> sDecibelsNormalisableRange = {
            -6.0f,
            6.0f
    };

    explicit AmplifierEqualiser();

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();
    
    void setResonanceDecibels(float newValue);
    void setBassDecibels(float newValue);
    void setMiddleDecibels(float newValue);
    void setTrebleDecibels(float newValue);
    void setPresenceDecibels(float newValue);

private:
    inline static const std::array<float, 5> sFrequencies = { 100.0f, 200.0f, 1000.0f, 5000.0f, 10000.0f };
    inline static const std::array<float, 5> sQualities =   { 1.5f,   1.5f,   1.5f,    1.5f,    1.5f };

    float mCurrentSampleRate = 44100.0f;

    std::array<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>, 5> mFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmplifierEqualiser)
};
