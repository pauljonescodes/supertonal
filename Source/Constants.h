#pragma once

#include <string>

namespace constants
{
    static const std::string pvtsIdentifier = "Supertonal";

    static const float blendMinimumValue = 0.0f;
    static const float blendMaximumValue = 1.0f;
    static const float blendIntervalValue = 0.01f;

    static const float gainDefaultValue = 0.0f;
    static const float gainMinimumValue = -30.f;
    static const float gainMaximumValue = 30.0f;
    static const float gainIntervalValue = 0.0001f;

    static const float biasDefaultValue = 0.0f;
    static const float biasMinimumValue = -1.0f;
    static const float biasMaximumValue = 1.0f;
    static const float biasIntervalValue = 0.01f;

    static const float qualityDefaultValue = 0.7f;
    static const float qualityMinimumValue = 0.1f;
    static const float qualityMaximumValue = 10.f;
    static const float qualityIntervalValue = 0.01f;

    static const float lowShelfFrequencyDefaultValue = 800.0f;
    static const float peakFilterFrequencyDefaultValue = 1000.0f;
    static const float highShelfFrequencyDefaultValue = 10000.0f;

    static const float frequencyMinimumValue = 15.0f;
    static const float frequencyMaximumValue = 20000.0f;
    static const float frequencyIntervalValue = 1.0f;

    static const std::string inputGainParameterId = "input-gain";
    static const std::string inputGainParameterName = "Input gain";

    static const std::string overdriveBiasParameterId = "overdrive-bias";
    static const std::string overdriveBiasParameterName = "Overdrive bias";

    static const std::string overdriveTanhWaveShaperBypassParameterId = "overdrive-tanh-shaper-bypass";
    static const std::string overdriveTanhWaveShaperBypassParameterName = "Overdrive tanh shaper bypass";

    static const std::string overdriveSoftClipWaveShaperBypassParameterId = "overdrive-soft-clip-bypass";
    static const std::string overdriveSoftClipWaveShaperBypassParameterName = "Overdrive soft clip bypass";

    static const std::string overdriveGainParameterId = "overdrive-gain";
    static const std::string overdriveGainParameterName = "Overdrive gain";

    static const std::string powerAmpGainParameterId = "power-amp-gain";
    static const std::string powerAmpGainParameterName = "Amp gain";

    static const std::string powerAmpLowShelfFrequencyParameterId = "power-amp-low-shelf-frequency";
    static const std::string powerAmpLowShelfFrequencyParamName = "Amp low shelf freq";

    static const std::string powerAmpLowShelfQualityParameterId = "power-amp-low-shelf-q";
    static const std::string powerAmpLowShelfQualityParameterName = "Amp low shelf Q";

    static const std::string powerAmpLowShelfGainParameterId = "power-amp-low-shelf-gain";
    static const std::string powerAmpLowShelfGainParameterName = "Amp low shelf gain";

    static const std::string powerAmpPeakFilterFrequencyParameterId = "power-amp-peak-filter-frequency";
    static const std::string powerAmpPeakFilterFrequencyParameterName = "Amp peak filter frequency";

    static const std::string powerAmpPeakFilterQualityParameterId = "power-amp-peak-filter-q";
    static const std::string powerAmpPeakFilterQualityParameterName = "Amp peak filter Q";

    static const std::string powerAmpPeakFilterGainParameterId = "power-amp-peak-filter-gain";
    static const std::string powerAmpPeakFilterGainParameterName = "Amp peak filter gain";

    static const std::string powerAmpHighShelfFrequencyParameterId = "power-amp-high-shelf-frequency";
    static const std::string powerAmpHighShelfFrequencyParameterName = "Amp high shelf frequency";

    static const std::string powerAmpHighShelfQualityParameterId = "power-amp-high-shelf-q";
    static const std::string powerAmpHighShelfQualityParameterName = "Amp high shelf Q";

    static const std::string powerAmpHighShelfGainParameterId = "power-amp-high-shelf-gain";
    static const std::string powerAmpHighShelfGainParameterName = "Amp high shelf gain";

    static const float reverbBlendDefaultValue = 0.1;
    static const std::string reverbBlendParameterId = "reverb-blend-parameter-id";
    static const std::string reverbBlendParameterName = "Reverb blend";

    static const std::string outputGainParameterId = "output-gain";
    static const std::string outputGainParameterName = "Output gain";
}