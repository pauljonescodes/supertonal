/*
  ==============================================================================

    SIMDMath.h
    Created: 10 Feb 2024 7:32:35pm
    Author:  paulm

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace SIMDMath
{
    inline float sum(const float* src, int num)
    {
        if (juce::SystemStats::hasSSE3())
        {
            __m128 mmSum = _mm_setzero_ps();

            int i = 0;
            int rounddown = num - (num % 16);

            for (; i < rounddown; i += 16)
            {
                __m128 v0 = _mm_loadu_ps(src + i + 0);
                __m128 v1 = _mm_loadu_ps(src + i + 4);
                __m128 s01 = _mm_add_ps(v0, v1);

                __m128 v2 = _mm_loadu_ps(src + i + 8);
                __m128 v3 = _mm_loadu_ps(src + i + 12);
                __m128 s23 = _mm_add_ps(v2, v3);

                mmSum = _mm_add_ps(mmSum, s01);
                mmSum = _mm_add_ps(mmSum, s23);
            }

            for (; i < num; i++)
                mmSum = _mm_add_ss(mmSum, _mm_load_ss(src + i));

            mmSum = _mm_hadd_ps(mmSum, mmSum);
            mmSum = _mm_hadd_ps(mmSum, mmSum);
            return _mm_cvtss_f32(mmSum);
        }

        // Fallback
        float s = 0;
        const float* e = src + num;

        while (src != e)
        {
            s += *src;
            src++;
        }
        return s;
    }
}
