/*
  ==============================================================================

    This code is based on the contents of the book: "Audio Effects: Theory,
    Implementation and Application" by Joshua D. Reiss and Andrew P. McPherson.

    Code by Juan Gil <https://juangil.com/>.
    Copyright (C) 2017-2020 Juan Gil.

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

#define _USE_MATH_DEFINES
#include <cmath>
#include <JuceHeader.h>

using namespace juce;

class Flanger
{
public:
    static constexpr float delayMinimumValue = 1.0f * 0.001f;
    static constexpr float delayMaximumValue = 20.0f * 0.001f;
    static constexpr float delayIntervalValue = 0.00001f;
    static constexpr float delayDefaultValue = 2.5f * 0.001f;
    static inline const juce::NormalisableRange<float> delayNormalisableRange =
        juce::NormalisableRange<float>(
            delayMinimumValue,
            delayMaximumValue,
            delayIntervalValue
            );

    static constexpr float widthMinimumValue = 1.0f * 0.001f;
    static constexpr float widthMaximumValue = 20.0f * 0.001f;
    static constexpr float widthDefaultValue = 10.0f * 0.001f;
    static constexpr float widthIntervalValue = 0.001f;
    static inline const juce::NormalisableRange<float> widthNormalisableRange = juce::NormalisableRange<float>(
        widthMinimumValue,
        widthMaximumValue,
        widthIntervalValue);

    static constexpr float depthMinimumValue = 0.0f;
    static constexpr float depthMaximumValue = 1.0f;
    static constexpr float depthDefaultValue = 1.0f;
    static constexpr float depthIntervalValue = 0.01f;
    static inline const juce::NormalisableRange<float> depthNormalisableRange = juce::NormalisableRange<float>(
        depthMinimumValue,
        depthMaximumValue,
        depthIntervalValue);

    static constexpr float feedbackMinimumValue = 0.0f;
    static constexpr float feedbackMaximumValue = 0.5f;
    static constexpr float feedbackDefaultValue = 0.0f;
    static constexpr float feedbackIntervalValue = 0.01f;
    static inline const juce::NormalisableRange<float> feedbackNormalisableRange = juce::NormalisableRange<float>(
        feedbackMinimumValue,
        feedbackMaximumValue,
        feedbackIntervalValue);

    static constexpr float frequencyMinimumValue = 0.05f;
    static constexpr float frequencyMaximumValue = 2.0f;
    static constexpr float frequencyDefaultValue = 0.2f;
    static constexpr float frequencyIntervalValue = 0.01f;
    static inline const juce::NormalisableRange<float> frequencyNormalisableRange = juce::NormalisableRange<float>(
        frequencyMinimumValue,
        frequencyMaximumValue,
        frequencyIntervalValue);

    Flanger() = default;

    void prepare (juce::dsp::ProcessSpec& spec)
    {
        setDelay(delayDefaultValue);
        setWidth(widthDefaultValue);
        setDepth(depthDefaultValue);
        setFeedback(feedbackDefaultValue);
        setFrequency(frequencyDefaultValue);

        mCurrentSampleRate = spec.sampleRate;

        float maxDelayTime = (delayMaximumValue + widthMaximumValue) * 1000;
        mDelayBufferSamples = (int)(maxDelayTime * (float)mCurrentSampleRate) + 1;
        if (mDelayBufferSamples < 1)
            mDelayBufferSamples = 1;

        mDelayBufferChannels = spec.numChannels;
        mDelayBuffer.setSize(mDelayBufferChannels, mDelayBufferSamples);
        mDelayBuffer.clear();

        mDelayWritePosition = 0;
        mLfoPhase = 0.0f;
        mInverseSampleRate = 1.0f / mCurrentSampleRate;
        mTwoPi = 2.0f * M_PI;
    };

    void reset() {
    };
    void process(juce::AudioBuffer<float>& buffer)
    {
        if (mIsBypassed)
        {
            return;
        }

        ScopedNoDenormals noDenormals;

        const int numInputChannels = buffer.getNumChannels();
        const int numOutputChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        int localWritePosition;
        float phase;
        float phaseMain;

        for (int channel = 0; channel < numInputChannels; ++channel) {
            float* channelData = buffer.getWritePointer(channel);
            float* delayData = mDelayBuffer.getWritePointer(channel);
            localWritePosition = mDelayWritePosition;
            phase = mLfoPhase;
            if (mStereo && channel != 0)
                phase = fmodf(phase + 0.25f, 1.0f);

            for (int sample = 0; sample < numSamples; ++sample) {
                float currentDelay = mDelaySmoothedValue.getNextValue(); //0.002;
                float currentWidth = mWidthSmoothedValue.getNextValue();// 0.01
                float currentDepth = mDepthSmoothedValue.getNextValue();// 1.0;
                float currentFeedback = mFeedbackSmoothedValue.getNextValue();// 0.0f;
                float currentInverted = true;
                float currentFrequency = mFrequencySmoothedValue.getNextValue();// 0.2f;

                const float in = channelData[sample];
                float out = 0.0f;

                float localDelayTime =
                    (currentDelay + currentWidth * lfo(phase, mWaveform)) * mCurrentSampleRate;

                float readPosition =
                    fmodf((float)localWritePosition - localDelayTime + (float)mDelayBufferSamples, mDelayBufferSamples);
                int localReadPosition = floorf(readPosition);

                switch (0) {
                case interpolationNearestNeighbour: {
                    float closestSample = delayData[localReadPosition % mDelayBufferSamples];
                    out = closestSample;
                    break;
                }
                case interpolationLinear: {
                    float fraction = readPosition - (float)localReadPosition;
                    float delayed0 = delayData[(localReadPosition + 0)];
                    float delayed1 = delayData[(localReadPosition + 1) % mDelayBufferSamples];
                    out = delayed0 + fraction * (delayed1 - delayed0);
                    break;
                }
                case interpolationCubic: {
                    float fraction = readPosition - (float)localReadPosition;
                    float fractionSqrt = fraction * fraction;
                    float fractionCube = fractionSqrt * fraction;

                    float sample0 = delayData[(localReadPosition - 1 + mDelayBufferSamples) % mDelayBufferSamples];
                    float sample1 = delayData[(localReadPosition + 0)];
                    float sample2 = delayData[(localReadPosition + 1) % mDelayBufferSamples];
                    float sample3 = delayData[(localReadPosition + 2) % mDelayBufferSamples];

                    float a0 = -0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
                    float a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
                    float a2 = -0.5f * sample0 + 0.5f * sample2;
                    float a3 = sample1;
                    out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
                    break;
                }
                }

                channelData[sample] = in + out * currentDepth * currentInverted;
                delayData[localWritePosition] = in + out * currentFeedback;

                if (++localWritePosition >= mDelayBufferSamples)
                    localWritePosition -= mDelayBufferSamples;

                phase += currentFrequency * mInverseSampleRate;
                if (phase >= 1.0f)
                    phase -= 1.0f;
            }

            if (channel == 0)
                phaseMain = phase;
        }

        mDelayWritePosition = localWritePosition;
        mLfoPhase = phaseMain;

        //======================================

        for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
            buffer.clear(channel, 0, numSamples);
    };

    void setBypassed(bool newValue)
    {
        mIsBypassed = newValue;
    }

    void setDelay(float newValue)
    {
        mDelaySmoothedValue.setTargetValue(newValue);
    }

    void setWidth(float newValue)
    {
        mWidthSmoothedValue.setTargetValue(newValue);
    }

    void setDepth(float newValue)
    {
        mDepthSmoothedValue.setTargetValue(newValue);
    }

    void setFeedback(float newValue)
    {
        mFeedbackSmoothedValue.setTargetValue(newValue);
    }

    void setFrequency(float newValue)
    {
        mFrequencySmoothedValue.setTargetValue(newValue);
    }

private:   
    enum waveformIndex {
        waveformSine = 0,
        waveformTriangle,
        waveformSawtooth,
        waveformInverseSawtooth,
    };

    enum interpolationIndex {
        interpolationNearestNeighbour = 0,
        interpolationLinear,
        interpolationCubic,
    };

    AudioSampleBuffer mDelayBuffer;
    int mDelayBufferSamples;
    int mDelayBufferChannels;
    int mDelayWritePosition;

    float mLfoPhase;
    float mInverseSampleRate;
    float mTwoPi;
    float mCurrentSampleRate = 441000;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mDelaySmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mWidthSmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mDepthSmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mFeedbackSmoothedValue;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mFrequencySmoothedValue;

    bool mInverted = false;
    int mWaveform = 0;  // (parameters, "LFO Waveform", waveformItemsUI, waveformSine)
    int mInterpolation = 0; // (parameters, "Interpolation", interpolationItemsUI, interpolationLinear)
    bool mStereo = false;
    bool mIsBypassed = false;

    float lfo (float phase, int waveform) 
    {
        float out = 0.0f;

        switch (waveform) {
        case waveformSine: {
            out = 0.5f + 0.5f * sinf(mTwoPi * phase);
            break;
        }
        case waveformTriangle: {
            if (phase < 0.25f)
                out = 0.5f + 2.0f * phase;
            else if (phase < 0.75f)
                out = 1.0f - 2.0f * (phase - 0.25f);
            else
                out = 2.0f * (phase - 0.75f);
            break;
        }
        case waveformSawtooth: {
            if (phase < 0.5f)
                out = 0.5f + phase;
            else
                out = phase - 0.5f;
            break;
        }
        case waveformInverseSawtooth: {
            if (phase < 0.5f)
                out = 0.5f - phase;
            else
                out = 1.5f - phase;
            break;
        }
        }

        return out;
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Flanger)
};
