/*
  ==============================================================================

    This code is based on the Jatin Chowdry's "BYOD"

    https://github.com/Chowdhury-DSP/BYOD

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Krusher.h"

class Bitcrusher
{
public:

    static constexpr float sampleRateMinimumValue = 1000.0f;
    static constexpr float sampleRateMaximumValue = 48000.0f;
    static constexpr float sampleRateIntervalValue = 1000.0f;
    static constexpr float sampleRateDefaultValue = 8000.0f;
    static inline const juce::NormalisableRange<float> sampleRateNormalisableRange =
        juce::NormalisableRange<float>(
            sampleRateMinimumValue,
            sampleRateMaximumValue,
            sampleRateIntervalValue);

    static constexpr float bitDepthMinimumValue = 1.0f;
    static constexpr float bitDepthMaximumValue = 12.0f;
    static constexpr float bitDepthIntervalValue = 1.0f;
    static constexpr float bitDepthDefaultValue = 4.0f;
    static inline const juce::NormalisableRange<float> bitDepthNormalisableRange =
        juce::NormalisableRange<float>(
            bitDepthMinimumValue,
            bitDepthMaximumValue,
            bitDepthIntervalValue);

    Bitcrusher() : mDCBlockerHPF(juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>())
    {};

    void prepare(juce::dsp::ProcessSpec& spec)
    {
        mCurrentSampleRate = spec.sampleRate;

        krusher_init_lofi_resample(&resample_state);

        mDCBlockerHPF.prepare(spec);
        *mDCBlockerHPF.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
            spec.sampleRate,
            15.0f,
            0.70710678118654752440f);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        if (mIsBypassed)
        {
            return;
        }

        krusher_bit_reduce_process_block(
            const_cast<float**> (buffer.getArrayOfWritePointers()),
            buffer.getNumChannels(),
            buffer.getNumSamples(),
            1, // "Zero-Order", "First-Order", "Second-Order", "Third-Order" 
            mBitDepth,
            brFilterStates.data());

        if (mTargetSampleRate >= mCurrentSampleRate)
        {
            return;
        }

        krusher_process_lofi_downsample(nullptr,
            &resample_state,
            const_cast<float**> (buffer.getArrayOfWritePointers()),
            buffer.getNumChannels(),
            buffer.getNumSamples(),
            double(mCurrentSampleRate / mTargetSampleRate));

        auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
        auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

        mDCBlockerHPF.process(processContext);
    }

    void reset()
    {
        mDCBlockerHPF.reset();
    }

    void setTargetSampleRate(float newValue)
    {
        mTargetSampleRate = newValue;
    }

    void setBitDepth(float newValue)
    {
        mBitDepth = newValue;
    }

    void setIsBypassed(bool newValue)
    {
        mIsBypassed = newValue;
    }

    bool getIsBypassed()
    {
        return mIsBypassed;
    }

private:
    float mCurrentSampleRate = 48000.0f;

    bool mIsBypassed = false;
    float mTargetSampleRate = 8000.0f; // createNormalisableRange(1000.0f, 48000.0f, 8000.0f)
    float mBitDepth = 4.0f; // NormalisableRange{ 1.0f, 12.0f, 1.0f }

    KrusherLofiResampleState resample_state{};
    std::array<KrusherBitReducerFilterState, 2> brFilterStates{};

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mDCBlockerHPF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Bitcrusher)
};

