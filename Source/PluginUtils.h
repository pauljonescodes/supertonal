/*
  ==============================================================================

    PluginUtils.h
    Created: 29 Dec 2023 8:14:17pm
    Author:  paulm

  ==============================================================================
*/

#pragma once

#include "PluginAudioParameters.h"

#include <string>

class PluginUtils {
public:
    static bool isNumeric(const std::string& str);
    static bool isToggleId(const std::string& str);
    static bool isModeId(const std::string& str);
    static bool isWaveshaperId(const std::string& str);
    static std::string toSnakeCase(const std::string& str);
    static std::string toTitleCase(const std::string& str);
};