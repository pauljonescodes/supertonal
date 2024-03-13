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

class Chorus
{
public:

	static constexpr float delayMinimumValue = 10.0f * 0.001f;
	static constexpr float delayMaximumValue = 50.0f * 0.001f;
	static constexpr float delayIntervalValue = 0.001f;
	static constexpr float delayDefaultValue = 30.0f * 0.001f;
	static inline const juce::NormalisableRange<float> delayNormalisableRange =
		juce::NormalisableRange<float>(
			delayMinimumValue,
			delayMaximumValue,
			delayIntervalValue
			);

	static constexpr float widthMinimumValue = 10.0f * 0.001f;
	static constexpr float widthMaximumValue = 50.0f * 0.001f;
	static constexpr float widthDefaultValue = 20.0f * 0.001f;
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

	static constexpr float frequencyMinimumValue = 0.05f;
	static constexpr float frequencyMaximumValue = 2.0f;
	static constexpr float frequencyDefaultValue = 0.2f;
	static constexpr float frequencyIntervalValue = 0.01f;
	static inline const juce::NormalisableRange<float> lfoFrequencyNormalisableRange = juce::NormalisableRange<float>(
		frequencyMinimumValue,
		frequencyMaximumValue,
		frequencyIntervalValue);

	Chorus() = default;

	void prepare(juce::dsp::ProcessSpec& spec)
	{
		setDelay(delayDefaultValue);
		setWidth(widthDefaultValue);
		setDepth(depthDefaultValue);
		setFrequency(frequencyDefaultValue);

		mCurrentSampleRate = spec.sampleRate;

		float maxDelayTime = (delayMaximumValue + widthMaximumValue) * 1000;
		delayBufferSamples = (int)(maxDelayTime * mCurrentSampleRate) + 1;
		if (delayBufferSamples < 1)
			delayBufferSamples = 1;

		delayBufferChannels = spec.numChannels;
		delayBuffer.setSize(delayBufferChannels, delayBufferSamples);
		delayBuffer.clear();

		delayWritePosition = 0;
		mLfoPhase = 0.0f;
		mInverseSampleRate = 1.0f / mCurrentSampleRate;
		twoPi = 2.0f * M_PI;
	};

	void reset()
	{
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

		float currentDelay = mDelaySmoothedValue.getNextValue();
		float currentWidth = mWidthSmoothedValue.getNextValue();
		float currentDepth = mDepthSmoothedValue.getNextValue();
		int numVoices = mNumVoices;
		float currentFrequency = mLFOFrequencySmoothedValue.getNextValue();
		bool stereo = mIsStereo;

		int localWritePosition;
		float phase;

		for (int channel = 0; channel < numInputChannels; ++channel) {
			float* channelData = buffer.getWritePointer(channel);
			float* delayData = delayBuffer.getWritePointer(channel);
			localWritePosition = delayWritePosition;
			phase = mLfoPhase;

			for (int sample = 0; sample < numSamples; ++sample) {
				const float in = channelData[sample];
				float out = 0.0f;
				float phaseOffset = 0.0f;
				float weight;

				for (int voice = 0; voice < numVoices - 1; ++voice) {
					if (stereo && numVoices > 2) {
						weight = (float)voice / (float)(numVoices - 2);
						if (channel != 0)
							weight = 1.0f - weight;
					}
					else {
						weight = 1.0f;
					}

					float localDelayTime =
						(currentDelay + currentWidth * lfo(phase + phaseOffset, mWaveform)) * mCurrentSampleRate;

					float readPosition =
						fmodf((float)localWritePosition - localDelayTime + (float)delayBufferSamples, delayBufferSamples);
					int localReadPosition = floorf(readPosition);

					switch (mInterpolation) {
					case interpolationNearestNeighbour: {
						float closestSample = delayData[localReadPosition % delayBufferSamples];
						out = closestSample;
						break;
					}
					case interpolationLinear: {
						float fraction = readPosition - (float)localReadPosition;
						float delayed0 = delayData[(localReadPosition + 0)];
						float delayed1 = delayData[(localReadPosition + 1) % delayBufferSamples];
						out = delayed0 + fraction * (delayed1 - delayed0);
						break;
					}
					case interpolationCubic: {
						float fraction = readPosition - (float)localReadPosition;
						float fractionSqrt = fraction * fraction;
						float fractionCube = fractionSqrt * fraction;

						float sample0 = delayData[(localReadPosition - 1 + delayBufferSamples) % delayBufferSamples];
						float sample1 = delayData[(localReadPosition + 0)];
						float sample2 = delayData[(localReadPosition + 1) % delayBufferSamples];
						float sample3 = delayData[(localReadPosition + 2) % delayBufferSamples];

						float a0 = -0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
						float a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
						float a2 = -0.5f * sample0 + 0.5f * sample2;
						float a3 = sample1;
						out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
						break;
					}
					}

					if (stereo && numVoices == 2)
						channelData[sample] = (channel == 0) ? in : out * currentDepth;
					else
						channelData[sample] += out * currentDepth * weight;

					if (numVoices == 3)
						phaseOffset += 0.25f;
					else if (numVoices > 3)
						phaseOffset += 1.0f / (float)(numVoices - 1);
				}

				delayData[localWritePosition] = in;

				if (++localWritePosition >= delayBufferSamples)
					localWritePosition -= delayBufferSamples;

				phase += currentFrequency * mInverseSampleRate;
				if (phase >= 1.0f)
					phase -= 1.0f;
			}
		}

		delayWritePosition = localWritePosition;
		mLfoPhase = phase;

		//======================================

		for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
			buffer.clear(channel, 0, numSamples);
	};

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

	void setFrequency(float newValue)
	{
		mLFOFrequencySmoothedValue.setTargetValue(newValue);
	}

	void setBypassed(bool newValue)
	{
		mIsBypassed = newValue;
	}

private:

	AudioSampleBuffer delayBuffer;
	int delayBufferSamples;
	int delayBufferChannels;
	int delayWritePosition;

	float mCurrentSampleRate = 441000;
	float mLfoPhase;
	float mInverseSampleRate;
	float twoPi;

	bool mIsStereo = false;
	bool mIsBypassed = false;
	int mNumVoices = 2;//(parameters, "Number of voices", {"2", "3", "4", "5"}, 0, [](float value){ return value + 2; })
	int mWaveform = 0; //(parameters, "LFO Waveform", waveformItemsUI, waveformSine)
	int mInterpolation = 0; //(parameters, "Interpolation", interpolationItemsUI, interpolationLinear)

	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mDelaySmoothedValue;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mWidthSmoothedValue;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mDepthSmoothedValue;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mLFOFrequencySmoothedValue;

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

	float lfo(float phase, int waveform) {
		float out = 0.0f;

		switch (waveform) {
		case waveformSine: {
			out = 0.5f + 0.5f * sinf(twoPi * phase);
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

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chorus)
};
