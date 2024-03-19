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

#include "AmplifierEqualiser.h"

AmplifierEqualiser::AmplifierEqualiser()
{
	*mFilters[0].state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
		mCurrentSampleRate,
		sFrequencies[0],
		sQualities[0],
		1.0f);
	*mFilters[1].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		mCurrentSampleRate,
		sFrequencies[1],
		sQualities[1],
		1.0f);
	*mFilters[2].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		mCurrentSampleRate,
		sFrequencies[2],
		sQualities[2],
		1.0f);
	*mFilters[3].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		mCurrentSampleRate,
		sFrequencies[3],
		sQualities[3],
		1.0f);
	*mFilters[4].state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
		mCurrentSampleRate,
		sFrequencies[4],
		sQualities[4],
		1.0f);
}

void AmplifierEqualiser::prepare(juce::dsp::ProcessSpec& spec)
{
	mCurrentSampleRate = spec.sampleRate;

	for (auto& filter : mFilters)
	{
		filter.prepare(spec);
	}
}

void AmplifierEqualiser::processBlock(juce::AudioBuffer<float>& buffer)
{
	auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
	auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

	for (std::size_t filterIndex = 0; filterIndex < mFilters.size(); ++filterIndex)
	{
		mFilters[filterIndex].process(processContext);
	}
}

void AmplifierEqualiser::reset()
{
	for (auto& filter : mFilters)
	{
		filter.reset();
	}
}

void AmplifierEqualiser::setResonanceDecibels(float newValue)
{
	*mFilters[0].state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
		mCurrentSampleRate,
		sFrequencies[0],
		sQualities[0],
		juce::Decibels::decibelsToGain(newValue));
}

void AmplifierEqualiser::setBassDecibels(float newValue)
{
	*mFilters[1].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		mCurrentSampleRate,
		sFrequencies[1],
		sQualities[1],
		juce::Decibels::decibelsToGain(newValue));
}

void AmplifierEqualiser::setMiddleDecibels(float newValue)
{
	*mFilters[2].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		mCurrentSampleRate,
		sFrequencies[2],
		sQualities[2],
		juce::Decibels::decibelsToGain(newValue));
}

void AmplifierEqualiser::setTrebleDecibels(float newValue)
{
	*mFilters[3].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
		mCurrentSampleRate,
		sFrequencies[3],
		sQualities[3],
		juce::Decibels::decibelsToGain(newValue));
}

void AmplifierEqualiser::setPresenceDecibels(float newValue)
{
	*mFilters[4].state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
		mCurrentSampleRate,
		sFrequencies[4],
		sQualities[4],
		juce::Decibels::decibelsToGain(newValue));
}