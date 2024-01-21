#include "MouseDrive.h"

MouseDrive::MouseDrive() :
    mHighPassFilter(juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>())
{
    mNetlistCircuitQuantities = std::make_unique<netlist::CircuitQuantityList>();
    mNetlistCircuitQuantities->addResistor(
        1.0e6f,
        "R2",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R2.setResistanceValue(self.value.load());
        },
        10.0e3f,
            2.0e6f);
    mNetlistCircuitQuantities->addResistor(
        1.0e3f,
        "R3",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R3.setResistanceValue(self.value.load());
        },
        100.0f,
            1.0e6f);
    mNetlistCircuitQuantities->addResistor(
        47.0f,
        "R4",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R4_C5.setResistanceValue(self.value.load());
        },
        10.0f,
            10.0e3f);
    mNetlistCircuitQuantities->addResistor(
        560.0f,
        "R5",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R5_C6.setResistanceValue(self.value.load());
        },
        10.0f,
            100.0e3f);
    mNetlistCircuitQuantities->addResistor(
        1.0e3f,
        "R6",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R6_C7.setResistanceValue(self.value.load());
        },
        100.0f,
            1.0e6f);
    mNetlistCircuitQuantities->addCapacitor(
        22.0e-9f,
        "C1",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.Vin_C1.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
    mNetlistCircuitQuantities->addCapacitor(
        1.0e-9f,
        "C2",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.C2.setCapacitanceValue(self.value.load());
        },
        1.0e-12f,
            1.0e-6f);
    mNetlistCircuitQuantities->addCapacitor(
        100.0e-12f,
        "C4",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.Rd_C4.setCapacitanceValue(self.value.load());
        },
        1.0e-12f,
            1.0e-6f);
    mNetlistCircuitQuantities->addCapacitor(
        2.2e-6f,
        "C5",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R4_C5.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
    mNetlistCircuitQuantities->addCapacitor(
        4.7e-6f,
        "C6",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R5_C6.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
    mNetlistCircuitQuantities->addCapacitor(
        4.7e-6f,
        "C7",
        [this](const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWaveDesignFilter)
            wdfModel.R6_C7.setCapacitanceValue(self.value.load());
        },
        100.0e-12f,
            1.0e-3f);
}

void MouseDrive::setDistortion(float targetValue)
{
    mDistortionSmoothedValue.setTargetValue(1.0f + MouseDriveWDF::Rdistortion * std::pow(targetValue, 5.0f));
};

void MouseDrive::setVolume(float targetValue)
{
    mVolumeSmoothedValue.setTargetValue(targetValue);
};

void MouseDrive::prepare(juce::dsp::ProcessSpec& spec)
{
    for (auto& model : mWaveDesignFilter)
        model.prepare(spec);

    mGain.setGainLinear(0.0f);
    mGain.prepare(spec);
    mGain.setRampDurationSeconds(0.05);

    mHighPassFilter.prepare(spec);

    *mHighPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
        spec.sampleRate,
        15.0f,
        0.70710678118654752440L);

    // pre-buffering
    AudioBuffer<float> buffer(2, spec.maximumBlockSize);
    for (int i = 0; i < 40000; i += spec.maximumBlockSize)
    {
        buffer.clear();
        processBlock(buffer);
    }
}

void MouseDrive::reset()
{
    mGain.reset();
    mHighPassFilter.reset();
}

void MouseDrive::processBlock(AudioBuffer<float>& buffer)
{
     mDistortionSmoothedValue.skip(buffer.getNumSamples());
     mVolumeSmoothedValue.skip(buffer.getNumSamples());
     const auto currentDistortionValue = mDistortionSmoothedValue.getCurrentValue();
     const auto currentVolumeValue = mVolumeSmoothedValue.getCurrentValue();

     for (auto [ch, data] : chowdsp::buffer_iters::channels(buffer))
     {
         if (mDistortionSmoothedValue.isSmoothing())
         {
             for (auto [n, x] : chowdsp::enumerate(data))
             {
                 mWaveDesignFilter[ch].Rd_C4.setResistanceValue(currentDistortionValue);
                 x = mWaveDesignFilter[ch].process(x);
             }
         }
         else
         {
             mWaveDesignFilter[ch].Rd_C4.setResistanceValue(currentDistortionValue);
             for (auto& x : data)
                 x = mWaveDesignFilter[ch].process(x);
         }
     }

    mGain.setGainDecibels(currentVolumeValue);

    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    mGain.process(processContext);
    mHighPassFilter.process(processContext);
}