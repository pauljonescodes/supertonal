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

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>

class BaseProcessor;
namespace netlist
{
struct CircuitQuantity
{
    enum Type
    {
        Resistance,
        Capacitance,
        Inductance,
    };

    using Setter = juce::dsp::FixedSizeFunction<32, void (const CircuitQuantity&)>;

    CircuitQuantity (CircuitQuantity&& other) noexcept;
    CircuitQuantity (float defaultVal, float minVal, float maxVal, Type qType, const std::string& name, Setter&& setterFunc);

    std::atomic<float> value;
    std::atomic_bool needsUpdate { false };

    const float defaultValue;
    const float minValue;
    const float maxValue;
    const Type type;
    const std::string name;

    Setter setter;
};

float fromString (const juce::String& str, const CircuitQuantity& q);

struct CircuitQuantityList
{
    auto begin() { return quantities.begin(); }
    auto begin() const { return quantities.begin(); }
    auto end() { return quantities.end(); }
    auto end() const { return quantities.end(); }
    auto size() const noexcept { return quantities.size(); }

    void addResistor (float defaultValue, const std::string& name, CircuitQuantity::Setter&& setter, float minVal = 100.0f, float maxVal = 10.0e6f);
    void addCapacitor (float defaultValue, const std::string& name, CircuitQuantity::Setter&& setter, float minVal = 0.1e-12f, float maxVal = 10.0f);
    void addInductor (float defaultValue, const std::string& name, CircuitQuantity::Setter&& setter, float minVal = 1.0e-3f, float maxVal = 100.0f);

    [[nodiscard]] const CircuitQuantity* findQuantity (const std::string&) const;

    std::vector<CircuitQuantity> quantities;
    struct SchematicSVGData
    {
        const char* data = nullptr;
        int size = 0;
    } schematicSVG;

    std::string extraNote;
};
} // namespace netlist