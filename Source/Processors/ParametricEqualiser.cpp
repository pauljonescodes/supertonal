#include "ParametricEqualiser.h"

ParametricEqualiser::ParametricEqualiser()
{
    // Initialize smoothed gain values and filters
    for (auto& decibelGainSmoothedValue : mDecibelGainSmoothedValues)
    {
        decibelGainSmoothedValue.reset(mCurrentSampleRate, 0.05); // Example settings
    }
}

void ParametricEqualiser::prepare(juce::dsp::ProcessSpec& spec)
{
    mCurrentSampleRate = spec.sampleRate;  // Store the sample rate

    for (auto& filter : mFilters)
    {
        filter.prepare(spec);
    }

    mLevelGain.prepare(spec);
}

void ParametricEqualiser::processBlock(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);

    for (std::size_t filterIndex = 0; filterIndex < mFilters.size(); ++filterIndex)
    {
        if (mDecibelGainSmoothedValues[filterIndex].isSmoothing())
        {
            float nextValue = mDecibelGainSmoothedValues[filterIndex].getNextValue();
            float linearGain = juce::Decibels::decibelsToGain(nextValue);

            if (filterIndex < 6)
            {
                *mFilters[filterIndex].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                    mCurrentSampleRate,
                    sFrequencies[filterIndex],
                    sQualities[filterIndex],
                    linearGain);
            }
            else
            {
                *mFilters[filterIndex].state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(
                    mCurrentSampleRate,
                    sFrequencies[filterIndex],
                    linearGain);
            }
        }

        mFilters[filterIndex].process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    if (mDecibelGainSmoothedValues[7].isSmoothing())
    {
        mLevelGain.setGainDecibels(mDecibelGainSmoothedValues[7].getNextValue());
    }

    mLevelGain.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void ParametricEqualiser::reset()
{
    for (auto& filter : mFilters)
    {
        filter.reset();
    }

    mLevelGain.reset();
}

void ParametricEqualiser::setGainDbAtIndex(float newGain, int index)
{
    if (index >= 0 && index < mDecibelGainSmoothedValues.size())
    {
        mDecibelGainSmoothedValues[index].setTargetValue(newGain);
    }
}