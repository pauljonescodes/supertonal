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

#include "TubeScreamer.h"

TubeScreamer::TubeScreamer () :
    mDCBlockerHPF(juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>())
{
    mNetlistCircuitQuantities = std::make_unique<netlist::CircuitQuantityList>();
    mNetlistCircuitQuantities->addResistor (
        4.7e3f,
        "R4",
        [this] (const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
                wdfModel.R4_ser_C3.setResistanceValue (self.value.load());
        },
        100.0f,
        25.0e3f);
    mNetlistCircuitQuantities->addResistor (10.0e3f,
                                           "R5",
                                           [this] (const netlist::CircuitQuantity& self)
                                           {
                                               for (auto& wdfModel : mWdf)
                                                   wdfModel.R5.setResistanceValue (self.value.load());
                                           });
    mNetlistCircuitQuantities->addCapacitor (
        1.0e-6f,
        "C2",
        [this] (const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
                wdfModel.Vin_C2.setCapacitanceValue (self.value.load());
        },
        100.0e-12f);
    mNetlistCircuitQuantities->addCapacitor (
        0.047e-6f,
        "C3",
        [this] (const netlist::CircuitQuantity& self)
        {
            for (auto& wdfModel : mWdf)
                wdfModel.R4_ser_C3.setCapacitanceValue (self.value.load());
        },
        1.0e-9f);
    mNetlistCircuitQuantities->addCapacitor (51.0e-12f,
                                            "C4",
                                            [this] (const netlist::CircuitQuantity& self)
                                            {
                                                for (auto& wdfModel : mWdf)
                                                    wdfModel.R6_P1_par_C4.setCapacitanceValue (self.value.load());
                                            });
}

void TubeScreamer::prepare (juce::dsp::ProcessSpec& spec)
{
    const auto driveGainValue = mDriveGainSmoothedValue.getCurrentValue();

    for (auto& toneProcessor : mTone)
    {
        toneProcessor.setSampleRate(spec.sampleRate);
    }

    auto gainParamSkew = (std::pow(10.0f, driveGainValue) - 1.0f) / 9.0f;
    for (auto& wdfProc : mWdf)
    {
        wdfProc.prepare (spec);
        wdfProc.setParameters (gainParamSkew, getDiodeIs(mDiodeType), mDiodeCount, true);
    }

    mDCBlockerHPF.prepare (spec);
    *mDCBlockerHPF.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
        spec.sampleRate,
        15.0f,
        0.70710678118654752440f);

    // pre-buffering
//    AudioBuffer<float> buffer (2, spec.maximumBlockSize);
//    for (int i = 0; i < 10000; i += spec.maximumBlockSize)
//    {
//        buffer.clear();
//        processBlock(buffer);
//    }
}

void TubeScreamer::processBlock(juce::AudioBuffer<float>& buffer)
{
    buffer.applyGain (juce::Decibels::decibelsToGain (-6.0f));

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    const auto driveGainValue = mDriveGainSmoothedValue.skip(numSamples);
    const auto levelGainValue = mLevelGainSmoothedValue.skip(numSamples);
    
    auto driveGainParamSkew = (std::pow (10.0f, driveGainValue) - 1.0f) / 9.0f;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        mWdf[ch].setParameters (driveGainParamSkew, getDiodeIs (mDiodeType), mDiodeCount);
        mWdf[ch].process (buffer.getWritePointer (ch), buffer.getNumSamples());
        mTone[ch].processBlock(buffer.getWritePointer(ch), buffer.getNumSamples());
    }

    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto processContext = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    mDCBlockerHPF.process (processContext);

    buffer.applyGain (juce::Decibels::decibelsToGain (levelGainValue));
}

void TubeScreamer::reset()
{
    mDCBlockerHPF.reset();
}

float TubeScreamer::getDiodeIs(int diodeType)
{
    switch (diodeType)
    {
    case 0: // GZ34
        return 2.52e-9f;
    case 1: // 1N34
        return 200.0e-12f;
    case 2: // 1N4148
        return 2.64e-9f;
    default:
        break;
    }

    jassertfalse;
    return 1.0e-9f;
}

void TubeScreamer::setDiodeType(int newDiodeType)
{
    mDiodeType = newDiodeType;
}

void TubeScreamer::setDiodeCount(int newDiodeCount)
{
    mDiodeCount = newDiodeCount;
}

void TubeScreamer::setDrive(float newGain)
{
    mDriveGainSmoothedValue.setTargetValue(newGain);
}

void TubeScreamer::setLevel(float newGain)
{
    mLevelGainSmoothedValue.setTargetValue(newGain);
}

void TubeScreamer::setTone(float newValue)
{
    for (auto& toneProcessor : mTone)
    {
        toneProcessor.setTone(newValue);
    }
}