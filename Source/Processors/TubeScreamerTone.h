/* https://github.com/a-carson/TubeScreamer-plugin */

#pragma once

#include <JuceHeader.h>

class TubeScreamerTone
{
public:

	TubeScreamerTone()
	{
	};

	/*Set sample rate in Hz*/
	void setSampleRate(float sampleRate)
	{
		c = 2.0f * sampleRate;
	}

	/*Set tone knob position 0 <= tone <= 1*/
	void setTone(float tone)
	{
		if (tone > 1.0)
			tone = 1.0;

		if (tone < 0.0)
			tone = 0.0;

		// Set Tone Potentiometer
		Rl = tone * 20e3;
		Rr = (1.0 - tone) * 20e3;

		// Combined impedences
		float wp = 1.0 / (Cs * (Rs * Ri / (Rs + Ri)));
		float wz = 1.0 / (Cz * (Rz + (Rl * Rr / (Rl + Rr))));
		float Y = (Rl + Rr) * (Rz + (Rl * Rr / (Rl + Rr)));
		float X = Rr / ((Rl + Rr) * (Rz + (Rl * Rr / (Rl + Rr))) * Cz);
		float W = Y / (Rl * Rf + Y);
		float V = (Rl * Rf + Y) / (Y * Rs * Cs);

		// Laplace domain coefficients
		float b0 = V * W * wz;
		float b1 = V;
		float a0 = wp * wz;
		float a1 = wp + wz + X;

		// Z-domain coefficients
		b[0] = b0 + b1 * c;
		b[1] = 2.0 * b0;
		b[2] = b0 - b1 * c;
		a[0] = a0 + a1 * c + pow(c, 2);
		a[1] = 2.0 * a0 - 2.0 * pow(c, 2);
		a[2] = a0 - a1 * c + pow(c, 2);

		// Normalise
		for (int i = 0; i < 3; i++)
		{
			b[i] /= a[0];
			a[2 - i] /= a[0];
		}

		// Set filter coefficients
		filter.setCoefficients(juce::IIRCoefficients(b[0], b[1], b[2], a[0], a[1], a[2]));
	}

	/*Process sample by sample*/
	float processSingleSample(float in)
	{
		auto out = b[0] * in + y1;

		JUCE_SNAP_TO_ZERO(out);

		y1 = b[1] * in - a[1] * out + y2;
		y2 = b[2] * in - a[2] * out;

		return out;
	}

	/*Process block of samples*/
	void processBlock(float* samples, int numSamples)
	{
		filter.processSamples(samples, numSamples);
	}

private:

	// Circuit parameters
	// Resistors
	float Rl = 20e3;
	float Rr = 0.0;
	float Rf = 1e3;
	float Ri = 10e3;
	float Rs = 1e3;
	float Rz = 220.0;

	// Capacitors
	float Cz = 0.22e-6;
	float Cs = 0.22e-6;

	// Filter Coefficients
	float b[3];
	float a[3];
	float y1 = 0.0f;
	float y2 = 0.0f;
	float c;				// for bilinear tranform

	juce::IIRFilter filter;
};