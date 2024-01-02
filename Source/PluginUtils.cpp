/*
  ==============================================================================

    PluginUtils.cpp
    Created: 29 Dec 2023 8:14:17pm
    Author:  paulm

  ==============================================================================
*/

#include "PluginUtils.h"

bool PluginUtils::isNumeric(const std::string& str) {
    return !str.empty() && std::find_if(str.begin(),
        str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}

bool PluginUtils::isToggleId(const std::string& str) 
{
    if (str.length() >= apvts::onComponentId.length()) 
    {
        return (0 == str.compare(str.length() - apvts::onComponentId.length(), apvts::onComponentId.length(), apvts::onComponentId));
    }
    else 
    {
        return false;
    }
}

bool PluginUtils::isModeId(const std::string& str)
{
    if (str.length() >= apvts::modeComponentId.length())
    {
        return (0 == str.compare(str.length() - apvts::modeComponentId.length(), apvts::modeComponentId.length(), apvts::modeComponentId));
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