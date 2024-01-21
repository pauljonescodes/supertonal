#pragma once

#include "JuceProcWrapper.h"
#include "MouseDriveWDF.h"
#include "BaseProcessor.h"
#include "CircuitQuantityHelper.h"
#include <JuceHeader.h>
#include <chowdsp_dsp_utils/chowdsp_dsp_utils.h>
#include <chowdsp_dsp_data_structures/chowdsp_dsp_data_structures.h>

using namespace juce;
using ParamLayout = AudioProcessorValueTreeState::ParameterLayout;

class MouseDrive 

{
public:
    MouseDrive(
        juce::AudioProcessorValueTreeState& apvts, 
        const juce::String& distortionParameterName, 
        const juce::String& volumeParameterName);

    void prepare(juce::dsp::ProcessSpec& spec);
    void processBlock(AudioBuffer<float>& buffer);

private:
    std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

    chowdsp::SmoothedBufferValue<float, juce::ValueSmoothingTypes::Multiplicative> mDistortionParam;
    chowdsp::FloatParameter* mVolumeParam = nullptr;

    MouseDriveWDF mWdf[2];
    chowdsp::Gain<float> mGain;
    chowdsp::FirstOrderHPF<float> mDcBlocker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MouseDrive)
};