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
#include "../../Utilities/OmegaProvider.h"
#include "chowdsp_wdf.h"

class TubeScreamerWDF
{
public:
    TubeScreamerWDF() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        Vin_C2.prepare ((float) spec.sampleRate);
        R4_ser_C3.prepare ((float)spec.sampleRate);
        R6_P1_par_C4.prepare ((float)spec.sampleRate);

        nDiodesSmooth.reset (spec.sampleRate, 0.01);
        gainSmooth.reset (spec.sampleRate, 0.01);
    }

    void setParameters (float gainParam, float diodeIs, float nDiodes, bool force = false)
    {
        curDiodeIs = diodeIs;
        if (force)
        {
            nDiodesSmooth.setCurrentAndTargetValue (nDiodes);
            gainSmooth.setCurrentAndTargetValue (gainParam);

            R6_P1_par_C4.setResistanceValue (Pot1 * gainSmooth.getTargetValue() + R6);
            dp.setDiodeParameters (curDiodeIs, Vt, nDiodesSmooth.getTargetValue());
        }
        else
        {
            nDiodesSmooth.setTargetValue (nDiodes);
            gainSmooth.setTargetValue (gainParam);
        }
    }

    inline float processSample (float x) noexcept
    {
        Vin_C2.setVoltage (x);

        dp.incident (P3.reflected());
        P3.incident (dp.reflected());

        return chowdsp::wdft::voltage<float> (RL);
    }

    void process (float* buffer, const int numSamples)
    {
        if (nDiodesSmooth.isSmoothing() || gainSmooth.isSmoothing())
        {
            for (int n = 0; n < numSamples; ++n)
            {
                R6_P1_par_C4.setResistanceValue (Pot1 * gainSmooth.getNextValue() + R6);
                dp.setDiodeParameters (curDiodeIs, Vt, nDiodesSmooth.getNextValue());

                buffer[n] = processSample (buffer[n]);
            }
            return;
        }

        R6_P1_par_C4.setResistanceValue (Pot1 * gainSmooth.getNextValue() + R6);
        dp.setDiodeParameters (curDiodeIs, Vt, nDiodesSmooth.getNextValue());
        for (int n = 0; n < numSamples; ++n)
            buffer[n] = processSample (buffer[n]);
    }

    // Port B
    chowdsp::wdft::CapacitiveVoltageSourceT<float> Vin_C2 { 1.0e-6f };
    chowdsp::wdft::ResistorT<float> R5 { 10.0e3f };
    chowdsp::wdft::WDFParallelT<float, decltype (Vin_C2), decltype (R5)> P1 { Vin_C2, R5 };

    // Port C
    chowdsp::wdft::ResistorCapacitorSeriesT<float> R4_ser_C3 { 4.7e3f, 0.047e-6f };

    // Port D
    chowdsp::wdft::ResistorT<float> RL { 1.0e6f };

    struct ImpedanceCalc
    {
        template <typename RType>
        static float calcImpedance (RType& R)
        {
            constexpr float Ag = 100.0f; // op-amp gain
            constexpr float Ri = 1.0e9f; // op-amp input impedance
            constexpr float Ro = 1.0e-1f; // op-amp output impedance

            const auto [Rb, Rc, Rd] = R.getPortImpedances();

            // This scattering matrix was derived using the R-Solver python script (https://github.com/jatinchowdhury18/R-Solver),
            // invoked with command: r_solver.py --adapt 0 --out scratch/tube_screamer_scatt.txt scratch/tube_screamer.txt
            R.setSMatrixData ({ { 0, (Ag * Rd * Ri - Rc * Rd + Rc * Ro) / ((Rb + Rc) * Rd + Rd * Ri - (Rb + Rc + Ri) * Ro), -((Ag + 1) * Rd * Ri + Rb * Rd - (Rb + Ri) * Ro) / ((Rb + Rc) * Rd + Rd * Ri - (Rb + Rc + Ri) * Ro), -Ro / (Rd - Ro) },
                                { -(Rb * Rc * Rd - Rb * Rc * Ro) / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro), ((Ag + 1) * Rc * Rc * Rd * Ri + (Ag + 1) * Rc * Rd * Ri * Ri - Rb * Rb * Rc * Rd + (Rb * Rb * Rc - (Rc + Rd) * Ri * Ri + (Rb * Rb - Rc * Rc) * Rd - (Rc * Rc + 2 * Rc * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), ((Ag + 1) * Rb * Rc * Rd * Ri + Rb * Rb * Rc * Rd - (Rb * Rb * Rc + 2 * (Rb * Rb + Rb * Rc) * Rd + (Rb * Rc + 2 * Rb * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), -Rb * Rc * Ro / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro) },
                                { -(Rb * Rc * Rd + Rc * Rd * Ri - (Rb * Rc + Rc * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro), (Ag * Rc * Rd * Ri * Ri + Rb * Rc * Rc * Rd + (Ag * Rb * Rc + (2 * Ag + 1) * Rc * Rc) * Rd * Ri - (Rb * Rc * Rc + 2 * (Rb * Rc + Rc * Rc) * Rd + (Rc * Rc + 2 * Rc * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), -((Ag + 1) * Rc * Rc * Rd * Ri + Rb * Rc * Rc * Rd - (Rb * Rc * Rc - Rd * Ri * Ri - (Rb * Rb - Rc * Rc) * Rd + (Rc * Rc - 2 * Rb * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd - (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro), -(Rb * Rc + Rc * Ri) * Ro / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro) },
                                { (Ag * Rc * Rd * Ri - ((Rb + Rc) * Rd + Rd * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro), ((Ag * Ag + 2 * Ag) * Rc * Rd * Rd * Ri * Ri + (2 * Ag * Rb * Rc + Ag * Rc * Rc) * Rd * Rd * Ri + (Rc * Rd * Ri + (Rb * Rc + Rc * Rc) * Rd) * Ro * Ro - ((Rb * Rc + Rc * Rc) * Rd * Rd + (2 * Ag * Rc * Rd + Ag * Rd * Rd) * Ri * Ri + ((Ag * Rb + (Ag + 1) * Rc) * Rd * Rd + (2 * Ag * Rb * Rc + Ag * Rc * Rc) * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd * Rd + (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro * Ro - ((Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd * Rd + ((Ag + 2) * Rc * Rd + Rd * Rd) * Ri * Ri + 2 * (Rb * Rb * Rc + Rb * Rc * Rc) * Rd + (2 * (Rb + Rc) * Rd * Rd + ((Ag + 4) * Rb * Rc + (Ag + 2) * Rc * Rc) * Rd) * Ri) * Ro), -(Ag * Rb * Rc * Rd * Rd * Ri + (Ag * Ag + Ag) * Rc * Rd * Rd * Ri * Ri - ((2 * Rb + Rc) * Rd * Ri + Rd * Ri * Ri + (Rb * Rb + Rb * Rc) * Rd) * Ro * Ro + ((Rb * Rb + Rb * Rc) * Rd * Rd - (Ag * Rc * Rd + (Ag - 1) * Rd * Rd) * Ri * Ri - (Ag * Rb * Rc * Rd + ((Ag - 2) * Rb + (Ag - 1) * Rc) * Rd * Rd) * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Rd * Ri * Ri + ((Ag + 2) * Rb * Rc + (Ag + 1) * Rc * Rc) * Rd * Rd * Ri + (Rb * Rb * Rc + Rb * Rc * Rc) * Rd * Rd + (Rb * Rb * Rc + Rb * Rc * Rc + (Rc + Rd) * Ri * Ri + (Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd + (2 * Rb * Rc + Rc * Rc + 2 * (Rb + Rc) * Rd) * Ri) * Ro * Ro - ((Rb * Rb + 2 * Rb * Rc + Rc * Rc) * Rd * Rd + ((Ag + 2) * Rc * Rd + Rd * Rd) * Ri * Ri + 2 * (Rb * Rb * Rc + Rb * Rc * Rc) * Rd + (2 * (Rb + Rc) * Rd * Rd + ((Ag + 4) * Rb * Rc + (Ag + 2) * Rc * Rc) * Rd) * Ri) * Ro), -((Ag + 1) * Rc * Rd * Rd * Ri + Rb * Rc * Rd * Rd - (Rb * Rc + Rc * Ri) * Ro * Ro - ((Rb + Rc) * Rd * Rd + Rd * Rd * Ri) * Ro) / ((Ag + 1) * Rc * Rd * Rd * Ri + Rb * Rc * Rd * Rd + (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro * Ro - (2 * Rb * Rc * Rd + (Rb + Rc) * Rd * Rd + ((Ag + 2) * Rc * Rd + Rd * Rd) * Ri) * Ro) } });

            const auto Ra = ((Ag + 1) * Rc * Rd * Ri + Rb * Rc * Rd - (Rb * Rc + (Rb + Rc) * Rd + (Rc + Rd) * Ri) * Ro) / ((Rb + Rc) * Rd + Rd * Ri - (Rb + Rc + Ri) * Ro);
            return Ra;
        }
    };

    chowdsp::wdft::RtypeAdaptor<float, 0, ImpedanceCalc, decltype (P1), decltype (R4_ser_C3), decltype (RL)> R { P1, R4_ser_C3, RL };

    // Port A
    static constexpr float Vt = 0.02585f;
    static constexpr auto R6 = 51.0e3f;
    static constexpr auto Pot1 = 500.0e3f;
    chowdsp::wdft::ResistorCapacitorParallelT<float> R6_P1_par_C4 { R6, 51.0e-12f };
    chowdsp::wdft::WDFParallelT<float, decltype (R6_P1_par_C4), decltype (R)> P3 { R6_P1_par_C4, R };

    chowdsp::wdft::DiodePairT<float, decltype (P3), chowdsp::wdft::DiodeQuality::Best, OmegaProvider> dp { P3, 4.352e-9f, Vt, 1.906f }; // 1N4148

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> nDiodesSmooth;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmooth;
    float curDiodeIs = 1.0e-9f;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeScreamerWDF)
};
