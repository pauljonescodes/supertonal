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

class Phaser
{
public:

    static constexpr float depthDefaultValue = 1.0f;
    static inline const juce::NormalisableRange<float> depthNormalisableRange = juce::NormalisableRange<float>(0.00f, 1.00f, 0.01f);

    static constexpr float feedbackDefaultValue = 0.7f;
    static inline const juce::NormalisableRange<float> feedbackNormalisableRange = juce::NormalisableRange<float>(0.00f, 0.90f, 0.01f);

    static constexpr float widthDefaultValue = 1000.00f;
    static inline const juce::NormalisableRange<float> widthNormalisableRange = juce::NormalisableRange<float>(50.00f, 3000.00f, 1.00f);

    static constexpr float frequencyDefaultValue = 0.05f;
    static inline const juce::NormalisableRange<float> frequencyNormalisableRange = juce::NormalisableRange<float>(0.00f, 2.00f, 0.01f);

    static constexpr float minimumFrequencyDefaultValue = 80.00f;
    static inline const juce::NormalisableRange<float> minimumFrequencyNormalisableRange = juce::NormalisableRange<float>(50.00f, 1000.00f, 1.00f);

    Phaser() = default;

    void prepare (juce::dsp::ProcessSpec& spec) {
        mNumFiltersPerChannel = mNumFilters;

        filters.clear();
        for (int i = 0; i < spec.numChannels * mNumFiltersPerChannel; ++i) {
            Filter* filter;
            filters.add(filter = new Filter());
        }

        filteredOutputs.clear();
        for (int i = 0; i < spec.numChannels; ++i)
            filteredOutputs.add(0.0f);

        mSampleCountToUpdateFilters = 0;
        mUpdateFiltersInterval = 32;

        mLfoPhase = 0.0f;
        mInverseSampleRate = 1.0f / (float)spec.sampleRate;
        mTwoPi = 2.0f * M_PI;


        mDepthSmoothedValue.setTargetValue(1.0f);
        mFeedbackSmoothedValue.setTargetValue(0.7f);
        mSweepWidthSmoothedValue.setTargetValue(1000.0f);
        mLFOFrequencySmoothedValue.setTargetValue(0.05f);
        mMinFrequencySmoothedValue.setTargetValue(80.0f);
    };

    void process (juce::AudioBuffer<float>& buffer) {
        if (mIsBypassed)
        {
            return;
        }

        ScopedNoDenormals noDenormals;

        const int numInputChannels = buffer.getNumChannels();
        const int numOutputChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        //======================================

        float phase;
        float phaseMain;
        unsigned int sampleCount;

        for (int channel = 0; channel < numInputChannels; ++channel) {
            float* channelData = buffer.getWritePointer(channel);
            sampleCount = mSampleCountToUpdateFilters;
            phase = mLfoPhase;
            if (mIsStereo && channel != 0)
                phase = fmodf(phase + 0.25f, 1.0f);

            for (int sample = 0; sample < numSamples; ++sample) {
                float in = channelData[sample];

                float centreFrequency = lfo(phase, mLFOWaveform);
                centreFrequency *= mSweepWidthSmoothedValue.getNextValue();
                centreFrequency += mMinFrequencySmoothedValue.getNextValue();

                phase += mLFOFrequencySmoothedValue.getNextValue() * mInverseSampleRate;
                if (phase >= 1.0f)
                    phase -= 1.0f;

                if (sampleCount++ % mUpdateFiltersInterval == 0)
                    updateFilters(centreFrequency);

                float filtered = in + mFeedbackSmoothedValue.getNextValue() * filteredOutputs[channel];
                for (int i = 0; i < mNumFilters; ++i)
                    filtered = filters[channel * mNumFilters + i]->processSingleSampleRaw(filtered);

                filteredOutputs.set(channel, filtered);
                float out = in + mDepthSmoothedValue.getNextValue() * (filtered - in) * 0.5f;
                channelData[sample] = out;
            }

            if (channel == 0)
                phaseMain = phase;
        }

        mSampleCountToUpdateFilters = sampleCount;
        mLfoPhase = phaseMain;

        for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
            buffer.clear(channel, 0, numSamples);
    };

    void reset() 
    {
    };

    void setDepth(float newValue)
    {
        mDepthSmoothedValue.setTargetValue(newValue);
    }

    void setFeedback(float newValue)
    {
        mFeedbackSmoothedValue.setTargetValue(newValue);
    }

    void setWidth(float newValue)
    {
        mSweepWidthSmoothedValue.setTargetValue(newValue);
    }

    void setFrequency(float newValue)
    {
        mLFOFrequencySmoothedValue.setTargetValue(newValue);
    }

    void setStereo(bool newValue)
    {
        mIsStereo = newValue;
    }

    void setMinimumFrequency(float newValue)
    {
        mMinFrequencySmoothedValue.setTargetValue(newValue);
    }

    void setBypassed(bool newValue)
    {
        mIsBypassed = newValue;
    }

private:

    enum waveformIndex {
        waveformSine = 0,
        waveformTriangle,
        waveformSquare,
        waveformSawtooth,
    };

    class Filter : public IIRFilter
    {
    public:
        void updateCoefficients (const double discreteFrequency) noexcept
        {
            jassert (discreteFrequency > 0);

            double wc = jmin (discreteFrequency, M_PI * 0.99);
            double tan_half_wc = tan (wc / 2.0);

            coefficients = IIRCoefficients (/* b0 */ tan_half_wc - 1.0,
                                            /* b1 */ tan_half_wc + 1.0,
                                            /* b2 */ 0.0,
                                            /* a0 */ tan_half_wc + 1.0,
                                            /* a1 */ tan_half_wc - 1.0,
                                            /* a2 */ 0.0);

            setCoefficients (coefficients);
        }
    };

    OwnedArray<Filter> filters;
    Array<float> filteredOutputs;

    int mNumFiltersPerChannel;
    unsigned int mSampleCountToUpdateFilters;
    unsigned int mUpdateFiltersInterval;

    float mLfoPhase;
    float mInverseSampleRate;
    float mTwoPi;

    bool mIsStereo = false;
    bool mIsBypassed = true;
    
    // ("Depth", "", 0.0f, 1.0f, 1.0f)
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mDepthSmoothedValue; 

    // ("Feedback", "", 0.0f, 0.9f, 0.7f)
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mFeedbackSmoothedValue;

    // ("Sweep width", "Hz", 50.0f, 3000.0f, 1000.0f)
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> mSweepWidthSmoothedValue;

    // ("LFO Frequency", "Hz", 0.0f, 2.0f, 0.05f)
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mLFOFrequencySmoothedValue; 

    // ("Min. Frequency", "Hz", 50.0f, 1000.0f, 80.0f)
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> mMinFrequencySmoothedValue; 
    
    // {"2", "4", "6", "8", "10"}
    int mNumFilters = 2; 

    // ("LFO Waveform", waveformItemsUI, waveformSine)
    int mLFOWaveform = 0; 

    void updateFilters(double centreFrequency)
    {
        double discreteFrequency = mTwoPi * centreFrequency * mInverseSampleRate;

        for (int i = 0; i < filters.size(); ++i)
            filters[i]->updateCoefficients(discreteFrequency);
    };
    
    float lfo(float phase, int waveform) {
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
        case waveformSquare: {
            if (phase < 0.5f)
                out = 1.0f;
            else
                out = 0.0f;
            break;
        }
        case waveformSawtooth: {
            if (phase < 0.5f)
                out = 0.5f + phase;
            else
                out = phase - 0.5f;
            break;
        }
        }

        return out;
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Phaser)
};
