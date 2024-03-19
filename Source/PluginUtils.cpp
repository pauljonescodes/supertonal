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

#include "PluginUtils.h"

bool PluginUtils::isNumeric(const std::string& str) {
    return !str.empty() && std::find_if(str.begin(),
        str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}

bool PluginUtils::isToggleId(const std::string& str) 
{
    const auto toggleId = "_" + apvts::onComponentId;
    if (str.length() >= toggleId.length())
    {
        return (0 == str.compare(str.length() - toggleId.length(), toggleId.length(), toggleId));
    }
    else 
    {
        return false;
    }
}

bool PluginUtils::isWaveshaperId(const std::string& str)
{
    if (str.length() >= apvts::waveshaperComponentId.length())
    {
        return (0 == str.compare(str.length() - apvts::waveshaperComponentId.length(), apvts::waveshaperComponentId.length(), apvts::waveshaperComponentId));
    }
    else
    {
        return false;
    }
}

std::string PluginUtils::toSnakeCase(const std::string& str) {
    std::string result;
    for (char ch : str) {
        if (ch == ' ' || ch == '-') {
            result += '_';
        }
        else {
            result += std::tolower(ch);
        }
    }
    return result;
}

std::string PluginUtils::toTitleCase(const std::string& str) {
    std::string result;
    bool nextUpper = false;

    int index = 0;
    for (char ch : str) {
        if (ch == '_' || ch == '-' ) {
            nextUpper = true;
            result += " ";
        }
        else if (nextUpper || index == 0) {
            result += std::toupper(ch);
            nextUpper = false; // Reset the flag
        }
        else {
            if (result.empty()) { // first letter in lowercase for camelCase
                result += std::tolower(ch);
            }
            else {
                result += ch; // preserve the original case for the rest
            }
        }
        index++;
    }

    return result;
}

void PluginUtils::checkForInvalidSamples(const juce::dsp::AudioBlock<float>& blockToCheck)
{
    auto numChans = blockToCheck.getNumChannels();
    auto numSamps = blockToCheck.getNumSamples();

    for (auto c = 0; c < numChans; ++c)
    {
        for (auto s = 0; s < numSamps; ++s)
        {
            auto sample = blockToCheck.getSample(c, s);
            jassert(!std::isnan(sample));
            jassert(sample <= 1.0f);
            jassert(sample >= -1.0f);
        }
    }
}

float PluginUtils::calculateRMS(juce::AudioBuffer<float>& buffer, int numChannels, int numSamples)
{
    float sum = 0.0f;

    for (int channel = 0; channel < numChannels; ++channel) {
        const float* channelData = buffer.getReadPointer(channel, 0);

        for (int i = 0; i < numSamples; ++i) {
            float sample = channelData[i];
            sum += sample * sample; // Square the sample and add it to the sum.
        }
    }

    float mean = sum / (numChannels * numSamples); // Calculate the mean of the squared sums.
    return std::sqrt(mean) + 0.0001; // Return the square root of the mean.
}