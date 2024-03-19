/*
    This code is part of the Supertonal guitar effects multi-processor.
    Copyright (C) 2023-2024  Paul Jones

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

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