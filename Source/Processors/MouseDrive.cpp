#include "MouseDrive.h"
#include "ParamUtils.h"


MouseDrive::MouseDrive(juce::AudioProcessorValueTreeState& apvts, const juce::String& distortionParameterName, const juce::String& volumeParameterName)
{
    using namespace ParameterHelpers;
    mDistortionParam.setParameterHandle(getParameterPointer<chowdsp::FloatParameter*>(apvts, distortionParameterName));
    mDistortionParam.setRampLength(0.025);
    mDistortionParam.mappingFunction = [](float x)
    {
        return 1.0f + MouseDriveWDF::Rdistortion * std::pow(x, 5.0f);
    };
    loadParameterPointer(mVolumeParam, apvts, volumeParameterName);

    mNetlistCircuitQuantities = std::make_unique<netlist::CircuitQuantityList>();
    mNetlistCircuitQuantities->addResistor(
        1.0e6f,
        "R2",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R2.setResistanceValue(self.value.load());
        },
        10.0e3f,
            2.0e6f);
    mNetlistCircuitQuantities->addResistor(
        1.0e3f,
        "R3",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R3.setResistanceValue(self.value.load());
        },
        100.0f,
            1.0e6f);
    mNetlistCircuitQuantities->addResistor(
        47.0f,
        "R4",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R4_C5.setResistanceValue(self.value.load());
        },
        10.0f,
            10.0e3f);
    mNetlistCircuitQuantities->addResistor(
        560.0f,
        "R5",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R5_C6.setResistanceValue(self.value.load());
        },
        10.0f,
            100.0e3f);
    mNetlistCircuitQuantities->addResistor(
        1.0e3f,
        "R6",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R6_C7.setResistanceValue(self.value.load());
        },
        100.0f,
            1.0e6f);
    mNetlistCircuitQuantities->addCapacitor(
        22.0e-9f,
        "C1",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.Vin_C1.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
    mNetlistCircuitQuantities->addCapacitor(
        1.0e-9f,
        "C2",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.C2.setCapacitanceValue(self.value.load());
        },
        1.0e-12f,
            1.0e-6f);
    mNetlistCircuitQuantities->addCapacitor(
        100.0e-12f,
        "C4",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.Rd_C4.setCapacitanceValue(self.value.load());
        },
        1.0e-12f,
            1.0e-6f);
    mNetlistCircuitQuantities->addCapacitor(
        2.2e-6f,
        "C5",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R4_C5.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
    mNetlistCircuitQuantities->addCapacitor(
        4.7e-6f,
        "C6",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R5_C6.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
    mNetlistCircuitQuantities->addCapacitor(
        4.7e-6f,
        "C7",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
            wdfModel.R6_C7.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
}

void MouseDrive::prepare(juce::dsp::ProcessSpec& spec)
{
    mDistortionParam.prepare(spec.sampleRate, spec.maximumBlockSize);
    for (auto& model : mWdf)
        model.prepare(spec);

    mGain.setGainLinear(0.0f);
    mGain.prepare(spec);
    mGain.setRampDurationSeconds(0.05);

    mDcBlocker.prepare(spec);
    mDcBlocker.calcCoefs(15.0f, (float)spec.sampleRate);

    // pre-buffering
    AudioBuffer<float> buffer(2, spec.maximumBlockSize);
    for (int i = 0; i < 40000; i += spec.maximumBlockSize)
    {
        buffer.clear();
        processBlock(buffer);
    }
}

void MouseDrive::processBlock(AudioBuffer<float>& buffer)
{
     mDistortionParam.process(buffer.getNumSamples());
     for (auto [ch, data] : chowdsp::buffer_iters::channels(buffer))
     {
         if (mDistortionParam.isSmoothing())
         {
             const auto* distParamSmoothData = mDistortionParam.getSmoothedBuffer();
             for (auto [n, x] : chowdsp::enumerate(data))
             {
                 mWdf[ch].Rd_C4.setResistanceValue(distParamSmoothData[n]);
                 x = mWdf[ch].process(x);
             }
         }
         else
         {
             mWdf[ch].Rd_C4.setResistanceValue(mDistortionParam.getCurrentValue());
             for (auto& x : data)
                 x = mWdf[ch].process(x);
         }
     }

    const auto volumeParamVal = mVolumeParam->getCurrentValue();
    if (volumeParamVal < 0.01f)
    {
        mGain.setGainLinear(0.0f);
    }
    else
    {
        mGain.setGainDecibels(-24.0f * (1.0f - volumeParamVal) - 12.0f);
    }

    mGain.process(buffer);

    mDcBlocker.processBlock(buffer);
}