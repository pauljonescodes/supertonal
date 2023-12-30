/*
  ==============================================================================

    PluginUtils.cpp
    Created: 29 Dec 2023 8:14:17pm
    Author:  paulm

  ==============================================================================
*/

#include "PluginUtils.h"

bool PluginUtils::isToggleId(const std::string& str) 
{
    std::string target = "on";
    if (str.length() >= target.length()) {
        // Compare the end of the string with "on"
        return (0 == str.compare(str.length() - target.length(), target.length(), target));
    }
    else {
        // The string is shorter than the target, so it can't end with "on"
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