#pragma once

#include "PluginAudioParameters.h"

#include <string>

class PluginUtils {
public:
    static bool isNumeric(const std::string& str);
    static bool isToggleId(const std::string& str);
    static bool isWaveshaperId(const std::string& str);
    static std::string toSnakeCase(const std::string& str);
    static std::string toTitleCase(const std::string& str);

    static void checkForInvalidSamples(const juce::dsp::AudioBlock<float>& blockToCheck);
    static float calculateRMS(juce::AudioBuffer<float>& buffer, int numChannels, int numSamples);

    static inline float calculateSamplesForBpmFractionAndRate(
        float beatsPerMinute, // ex. 120
        float fractionOfBeat, // ex. 4.0f
        const float samplesPerSecond // ex. 48,000
    ) {
        if (beatsPerMinute == 0.0f || fractionOfBeat == 0.0f)
        {
            return 0.0f;
        }

        float beatDurationSeconds = (60.0f / beatsPerMinute); // 0.5
        float samplesPerBeat = beatDurationSeconds * samplesPerSecond; // 24,000
        return samplesPerBeat / fractionOfBeat; // 200;
    }

    static inline float calculateSamplesForMilliseconds(
        const float samplesPerSecond, // ex. 48,000
        const float milliseconds // 1/1000th second, ex. 30
    ) {
        return (milliseconds * 0.001) * samplesPerSecond;
    }

    static inline float clampedValueForFractionOfBeat(
        float beatsPerMinute,
        float fractionOfBeat, float minimumValue = 0.000f, float maximumValue = 99.999f)
    {
        float noteDurationSeconds = 60.0f / beatsPerMinute * fractionOfBeat;
        float frequencyHz = 1.0f / noteDurationSeconds;
        return std::clamp(frequencyHz, minimumValue, maximumValue);
    }
};