/*
  ==============================================================================

    InstrumentEqualiser.cpp
    Created: 29 Jan 2024 10:37:15pm
    Author:  paulm

  ==============================================================================
*/

#include "InstrumentEqualiser.h"

InstrumentEqualiser::InstrumentEqualiser()
{
    // Initialize the filters with default values
    *mFilters[0].state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(mCurrentSampleRate, sLowPassFrequencyNormalisableRange.start);
    for (int i = 1; i <= 4; ++i)
    {
        *mFilters[i].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(mCurrentSampleRate, getDefaultValueForIndex(i), sQualityNormalisableRange.start, 1.0f);
    }
    *mFilters[5].state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(mCurrentSampleRate, sHighPassFrequencyNormalisableRange.start);
}

void InstrumentEqualiser::prepare(juce::dsp::ProcessSpec& spec)
{
    mCurrentSampleRate = spec.sampleRate;
    for (auto& filter : mFilters)
    {
        filter.prepare(spec);
    }
}

void InstrumentEqualiser::processBlock(juce::AudioBuffer<float>& buffer)
{
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    for (std::size_t filterIndex = 0; filterIndex < mFilters.size(); ++filterIndex)
    {
        if (!mBypasses[filterIndex])
        {
            mFilters[filterIndex].process(processContext);
        }
    }
}

void InstrumentEqualiser::reset()
{
    for (auto& filter : mFilters)
    {
        filter.reset();
    }
}

void InstrumentEqualiser::setOnAtIndex(bool newValue, int index)
{
    if (index >= 0 && index < mBypasses.size())
    {
        mBypasses[index] = !newValue; // a bypass is a "not on"
    }
}

void InstrumentEqualiser::setFrequencyAtIndex(float newValue, int index)
{
    if (index >= 0 && index < mFilters.size() && newValue != 0.0)
    {
        mFrequencies[index] = newValue;
        auto& filter = mFilters[index];
        const auto quality = mQualities[index];
        const auto gain = mDecibelGains[index];

        if (index == 0)
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(mCurrentSampleRate, newValue, quality);
        }
        else if (index == 5)
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(mCurrentSampleRate, newValue, quality);
        }
        else 
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(mCurrentSampleRate, newValue, quality, juce::Decibels::decibelsToGain(gain));
        }
    }
}

void InstrumentEqualiser::setGainAtIndex(float newValue, int index)
{
    if (index >= 0 && index < mFilters.size())
    {
        mDecibelGains[index] = newValue;
        auto& filter = mFilters[index];
        const auto frequency = mFrequencies[index];
        const auto quality = mQualities[index];

        if (index == 0 || index == 5)
        {
            // For high-pass and low-pass filters, gain doesn't affect the shape of the filter, so we don't do anything here.
            // If you want to apply gain, consider adding a separate gain stage after the filter.
        }
        else
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(mCurrentSampleRate, frequency, quality, juce::Decibels::decibelsToGain(newValue));
        }
    }
}

void InstrumentEqualiser::setQualityAtIndex(float newValue, int index)
{
    if (index >= 0 && index < mFilters.size() && newValue != 0.0)
    {
        mQualities[index] = newValue;
        auto& filter = mFilters[index];
        const auto frequency = mFrequencies[index];
        const auto gain = mDecibelGains[index];

        if (index == 0)
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(mCurrentSampleRate, frequency, newValue);
        }
        else if (index == 5)
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(mCurrentSampleRate, frequency, newValue);
        }
        else
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(mCurrentSampleRate, frequency, newValue, juce::Decibels::decibelsToGain(gain));
        }
    }
}

// Helper function to get default values for frequencies based on the filter index
float InstrumentEqualiser::getDefaultValueForIndex(int index)
{
    switch (index)
    {
    case 1: return InstrumentEqualiser::sLowPeakFrequencyDefaultValue;
    case 2: return InstrumentEqualiser::sLowMidPeakFrequencyDefaultValue;
    case 3: return InstrumentEqualiser::sHighMidPeakFrequencyDefaultValue;
    case 4: return InstrumentEqualiser::sHighPeakFrequencyDefaultValue;
    default: return 0.0f; // This shouldn't happen
    }
}

// Helper function to get the corresponding frequency range based on the filter index
const juce::NormalisableRange<float>& InstrumentEqualiser::getFrequencyNormalisableRangeForIndex(int index)
{
    switch (index)
    {
    case 0: return sLowPassFrequencyNormalisableRange;
    case 1: return sLowPeakFrequencyNormalisableRange;
    case 2: return sLowMidPeakFrequencyNormalisableRange;
    case 3: return sHighMidPeakFrequencyNormalisableRange;
    case 4: return sHighPeakFrequencyNormalisableRange;
    case 5: return sHighPassFrequencyNormalisableRange;
    default: return sLowPassFrequencyNormalisableRange; // This shouldn't happen
    }
}