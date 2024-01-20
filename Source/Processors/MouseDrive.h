#pragma once

#include "JuceProcWrapper.h"
#include "MouseDriveWDF.h"
#include "BaseProcessor.h"
#include <JuceHeader.h>
#include <chowdsp_dsp_utils/chowdsp_dsp_utils.h>
#include <chowdsp_dsp_data_structures/chowdsp_dsp_data_structures.h>

using namespace juce;
using ParamLayout = AudioProcessorValueTreeState::ParameterLayout;

class MouseDrive : public juce::AudioProcessor

{
public:
    MouseDrive(juce::AudioProcessorValueTreeState& apvts, const juce::String& distortionParameterName, const juce::String& volumeParameterNam);

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

    double getTailLengthSeconds() const override { return 0.0; }
    void releaseResources() override {}

    bool acceptsMidi() const final { return false; }
    bool producesMidi() const final { return false; }

    AudioProcessorEditor* createEditor() final { return nullptr; }
    bool hasEditor() const final { return false; }

    int getNumPrograms() final { return 0; }
    void setCurrentProgram(int /*index*/) final {}
    int getCurrentProgram() final { return 0; }

    const String getProgramName(int /*index*/) final { return {}; }
    void changeProgramName(int /*index*/, const String& /*newName*/) final {}

    void getStateInformation(MemoryBlock& /*destData*/) final {}
    void setStateInformation(const void* /*data*/, int /*sizeInBytes*/) final {}

    const juce::String getName() const override;

private:
    std::unique_ptr<netlist::CircuitQuantityList> mNetlistCircuitQuantities{};

    chowdsp::SmoothedBufferValue<float, juce::ValueSmoothingTypes::Multiplicative> mDistortionParam;
    chowdsp::FloatParameter* mVolumeParam = nullptr;

    MouseDriveWDF mWdf[2];
    chowdsp::Gain<float> mGain;
    chowdsp::FirstOrderHPF<float> mDcBlocker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MouseDrive)
};