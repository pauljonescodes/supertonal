#pragma once

#include <JuceHeader.h>
#include "Krusher.h"

class BitCrusher
{
public:
    BitCrusher() = default;

    void prepare(juce::dsp::ProcessSpec& spec)
    {
        mCurrentSampleRate = spec.sampleRate;

        krusher_init_lofi_resample(&resample_state);

        //dcBlocker.prepare(2);
        //dcBlocker.calcCoefs(20.0f, (float)sampleRate);
    }

    void processAudio(juce::AudioBuffer<float>& buffer)
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

        //dcBlocker.processBlock(buffer);
    }

    void reset()
    {

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

    Krusher_Lofi_Resample_State resample_state{};
    std::array<Krusher_Bit_Reducer_Filter_State, 2> brFilterStates{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BitCrusher)
};
