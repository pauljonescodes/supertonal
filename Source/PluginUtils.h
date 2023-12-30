/*
  ==============================================================================

    PluginUtils.h
    Created: 29 Dec 2023 8:14:17pm
    Author:  paulm

  ==============================================================================
*/

#pragma once

#include <string>

class PluginUtils {
public:
    static bool isToggleId(const std::string& str);
    static std::string toSnakeCase(const std::string& str);
    static std::string toTitleCase(const std::string& str);
};