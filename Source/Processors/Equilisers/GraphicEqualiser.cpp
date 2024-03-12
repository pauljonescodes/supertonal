#include "GraphicEqualiser.h"

GraphicEqualiser::GraphicEqualiser()
{   
    for (auto filterIndex = 0; filterIndex < sFrequencies.size(); ++filterIndex)
    {
        if (filterIndex < 6)
        {
            *mFilters[filterIndex].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                mCurrentSampleRate,
                sFrequencies[filterIndex],
                sQualities[filterIndex],
                1.0f);
        }
        else
        {
            *mFilters[filterIndex].state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                mCurrentSampleRate,
                sFrequencies[filterIndex],
                sQualities[filterIndex],
                1.0f);
        }
    } 
    
    mLevelGain.setGainDecibels(0.0f);
}

void GraphicEqualiser::prepare(juce::dsp::ProcessSpec& spec)
{
    mCurrentSampleRate = spec.sampleRate; 

    for (auto& filter : mFilters)
    {
        filter.prepare(spec);
    }

    mLevelGain.prepare(spec);
}

void GraphicEqualiser::processBlock(juce::AudioBuffer<float>& buffer)
{
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    for (std::size_t filterIndex = 0; filterIndex < mFilters.size(); ++filterIndex)
    {
        mFilters[filterIndex].process(processContext);
    }

    mLevelGain.process(processContext);
}

void GraphicEqualiser::reset()
{
    for (auto& filter : mFilters)
    {
        filter.reset();
    }

    mLevelGain.reset();
}

void GraphicEqualiser::setGainDecibelsAtIndex(float newGainDecibels, int index)
{

    if (index < 6)
    {
        *mFilters[index].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            mCurrentSampleRate,
            sFrequencies[index],
            sQualities[index],
            juce::Decibels::decibelsToGain(newGainDecibels));
    }
    else if (index == 6)
    {
        *mFilters[index].state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            mCurrentSampleRate,
            sFrequencies[index],
            sQualities[index],
            juce::Decibels::decibelsToGain(newGainDecibels));
    }
    else
    {
        mLevelGain.setGainDecibels(newGainDecibels);
    }
}