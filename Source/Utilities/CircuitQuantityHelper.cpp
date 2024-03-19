/*
  ==============================================================================

    This code is based on the Jatin Chowdry's "BYOD"

    https://github.com/Chowdhury-DSP/BYOD

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

#include "CircuitQuantityHelper.h"

namespace netlist
{
CircuitQuantity::CircuitQuantity (CircuitQuantity&& other) noexcept
    : value (other.value.load()),
      needsUpdate (other.needsUpdate.load()),
      defaultValue (other.defaultValue),
      minValue (other.minValue),
      maxValue (other.maxValue),
      type (other.type),
      name (other.name),
      setter (std::move (other.setter))
{
}

CircuitQuantity::CircuitQuantity (float defaultVal,
                                  float minVal,
                                  float maxVal,
                                  Type qType,
                                  const std::string& _name,
                                  Setter&& setterFunc)
    : value (defaultVal),
      defaultValue (defaultVal),
      minValue (minVal),
      maxValue (maxVal),
      type (qType),
      name (_name),
      setter (std::move (setterFunc))
{
}

float fromString (const juce::String& str, const CircuitQuantity& q)
{
    juce::String justTheNumbers {};
    juce::String suffix {};
    for (int i = 0; i < str.length(); ++i)
    {
        const auto indexOfNumber = str.indexOfAnyOf ("0123456789.", i);
        if (indexOfNumber < 0)
        {
            suffix = str.substring (i);
            break;
        }

        justTheNumbers += str[i];
    }

    const auto baseNumber = justTheNumbers.getFloatValue();
    const auto multiplier = [] (const juce::String& sfx) -> float
    {
        if (sfx.containsAnyOf ("kK"))
            return 1.0e3f;
        if (sfx.containsAnyOf ("M"))
            return 1.0e6f;
        if (sfx.containsAnyOf ("m"))
            return 1.0e-3f;
        if (sfx.containsAnyOf ("uμ"))
            return 1.0e-6f;
        if (sfx.containsAnyOf ("n"))
            return 1.0e-9f;
        if (sfx.containsAnyOf ("p"))
            return 1.0e-12f;
        return 1.0f;
    }(suffix);

    return juce::jlimit (q.minValue, q.maxValue, baseNumber * multiplier);
}

void CircuitQuantityList::addResistor (float defaultValue, const std::string& name, CircuitQuantity::Setter&& setter, float minVal, float maxVal)
{
    quantities.emplace_back (defaultValue,
                             minVal,
                             maxVal,
                             netlist::CircuitQuantity::Resistance,
                             name,
                             std::move (setter));
}

void CircuitQuantityList::addCapacitor (float defaultValue, const std::string& name, CircuitQuantity::Setter&& setter, float minVal, float maxVal)
{
    quantities.emplace_back (defaultValue,
                             minVal,
                             maxVal,
                             netlist::CircuitQuantity::Capacitance,
                             name,
                             std::move (setter));
}

void CircuitQuantityList::addInductor (float defaultValue, const std::string& name, CircuitQuantity::Setter&& setter, float minVal, float maxVal)
{
    quantities.emplace_back (defaultValue,
                             minVal,
                             maxVal,
                             netlist::CircuitQuantity::Inductance,
                             name,
                             std::move (setter));
}

const CircuitQuantity* CircuitQuantityList::findQuantity (const std::string& name) const
{
    for (const auto& q : quantities)
    {
        if (q.name == name)
            return &q;
    }
    return nullptr;
}
} // namespace netli
