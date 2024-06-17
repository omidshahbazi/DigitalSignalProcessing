#pragma once
#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Memory.h"
#include "../Debug.h"

template <typename T, uint8_t StageCount>
class BiquadFilter : public Filter<T, MIN_SAMPLE_RATE>
{
	static_assert(StageCount != 0, "StageCount cannot be 0");

public:
	struct Coefficients
	{
		double a0;
		double a1;
		double a2;
		double b0;
		double b1;
		double b2;
	};

private:
	struct Stage
	{
		Coefficients Coeffs;
		double z1;
		double z2;
	};

public:
	BiquadFilter()
		: m_Stages(Memory::Allocate<Stage>(StageCount)) {}

	~BiquadFilter()
	{
		Memory::Deallocate(m_Stages);
	}

	void SetCoefficients(const Coefficients *Values)
	{
		ASSERT(Values != nullptr, "Values cannot be null");

		for (uint8_t i = 0; i < StageCount; ++i)
		{
			m_Stages[i].Coeffs = Values[i];
		}
	}

	void Reset()
	{
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			m_Stages[i].z1 = 0;
			m_Stages[i].z2 = 0;
		}
	}

	T Process(T Value) override
	{
		if (m_Stages == nullptr)
		{
			return Value;
		}

		for (uint8_t i = 0; i < StageCount; ++i)
		{
			T Input = Value;
			Stage &stage = m_Stages[i];

			Value = (Input * stage.Coeffs.b0) + stage.z1;
			stage.z1 = (Input * stage.Coeffs.b1) + stage.z2 - (stage.Coeffs.a1 * Value);
			stage.z2 = (Input * stage.Coeffs.b2) - (stage.Coeffs.a2 * Value);
		}

		return Value;
	}

public:
	// Needs a 1 stage BiquadFilter
	// SampleRate [1, MAX_SAMPLE_RATE]
	// CutoffFrequency [1, MAX_FREQUENCY]
	// Resonance [0.1, 10] Common [0.7, 1]
	// - Values less than 0.7 result in a smoother transition near the cutoff frequency, reducing peak resonance but also making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the cutoff frequency, which can lead to oscillations or instability.
	static void SetLowPassFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, float Resonance = 1)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(1 <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate");
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency");
		ASSERT(0.1 <= Resonance && Resonance <= 10, "Invalid Resonance");

		ASSERT(Filter != nullptr, "Filter cannot be null");

		double Omega = 2.0 * Math::PI_VALUE * CutoffFrequency / SampleRate;
		double CosOmega = cos(Omega);
		double Alpha = sin(Omega) / (2.0 * Resonance);

		Coefficients Coeffs;
		Coeffs.b0 = (1.0 - CosOmega) / 2.0;
		Coeffs.b1 = 1.0 - CosOmega;
		Coeffs.b2 = (1.0 - CosOmega) / 2.0;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		// Normalize the coefficients
		Coeffs.b0 /= Coeffs.a0;
		Coeffs.b1 /= Coeffs.a0;
		Coeffs.b2 /= Coeffs.a0;
		Coeffs.a1 /= Coeffs.a0;
		Coeffs.a2 /= Coeffs.a0;

		Filter->SetCoefficients(&Coeffs);
	}

	// Needs a 1 stage BiquadFilter
	// SampleRate [1, MAX_SAMPLE_RATE]
	// CutoffFrequency [1, MAX_FREQUENCY]
	// Resonance [0.1, 10] Common [0.7, 1]
	// - Values less than 0.7 result in a smoother transition near the cutoff frequency, reducing peak resonance but also making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the cutoff frequency, which can lead to oscillations or instability.
	static void SetHighPassFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, float Resonance = 1)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(1 <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate");
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency");
		ASSERT(0.1 <= Resonance && Resonance <= 10, "Invalid Resonance");

		ASSERT(Filter != nullptr, "Filter cannot be null");

		double Omega = 2.0 * Math::PI_VALUE * CutoffFrequency / SampleRate;
		double CosOmega = cos(Omega);
		double Alpha = sin(Omega) / (2.0 * Resonance);

		Coefficients Coeffs;
		Coeffs.b0 = (1.0 + CosOmega) / 2.0;
		Coeffs.b1 = -(1.0 + CosOmega);
		Coeffs.b2 = (1.0 + CosOmega) / 2.0;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		// Normalize the coefficients
		Coeffs.b0 /= Coeffs.a0;
		Coeffs.b1 /= Coeffs.a0;
		Coeffs.b2 /= Coeffs.a0;
		Coeffs.a1 /= Coeffs.a0;
		Coeffs.a2 /= Coeffs.a0;

		Filter->SetCoefficients(&Coeffs);
	}

	// Needs a 1 stage BiquadFilter
	// SampleRate [1, MAX_SAMPLE_RATE]
	// CenterFrequency [1, MAX_FREQUENCY]
	// Bandwidth [1, MAX_FREQUENCY]
	// Resonance [0.1, 10] Common [0.7, 1]
	// - Values less than 0.7 result in a broader bandwidth and less peak resonance, making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the center frequency, which can lead to more pronounced resonance or oscillations.
	static void SetBandPassFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CenterFrequency, float Bandwidth, float Resonance = 1)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(1 <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate");
		ASSERT(0 < CenterFrequency && CenterFrequency <= MAX_FREQUENCY, "Invalid CenterFrequency");
		ASSERT(0 <= Bandwidth && Bandwidth <= MAX_FREQUENCY, "Invalid Bandwidth");
		ASSERT(0.1 <= Resonance && Resonance <= 10, "Invalid Resonance");

		ASSERT(Filter != nullptr, "Filter cannot be null");

		double Omega = 2.0 * Math::PI_VALUE * CenterFrequency / SampleRate;
		double CosOmega = cos(Omega);
		double Alpha = sin(Omega) / (2.0 * Resonance);

		Coefficients Coeffs;
		Coeffs.b0 = Alpha;
		Coeffs.b1 = 0.0;
		Coeffs.b2 = -Alpha;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		// Normalize the coefficients
		Coeffs.b0 /= Coeffs.a0;
		Coeffs.b1 /= Coeffs.a0;
		Coeffs.b2 /= Coeffs.a0;
		Coeffs.a1 /= Coeffs.a0;
		Coeffs.a2 /= Coeffs.a0;

		Filter->SetCoefficients(&Coeffs);
	}

	// Needs a 1 stage BiquadFilter
	// SampleRate [1, MAX_SAMPLE_RATE]
	// CenterFrequency [1, MAX_FREQUENCY]
	// Bandwidth [1, MAX_FREQUENCY]
	// Resonance [0.1, 10] Common [0.7, 1]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetBandStopFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CenterFrequency, float Bandwidth, float Resonance = 1)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(1 <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate");
		ASSERT(0 < CenterFrequency && CenterFrequency <= MAX_FREQUENCY, "Invalid CenterFrequency");
		ASSERT(0 <= Bandwidth && Bandwidth <= MAX_FREQUENCY, "Invalid Bandwidth");
		ASSERT(0.1 <= Resonance && Resonance <= 10, "Invalid Resonance");

		ASSERT(Filter != nullptr, "Filter cannot be null");

		double Omega = 2.0 * Math::PI_VALUE * CenterFrequency / SampleRate;
		double CosOmega = cos(Omega);
		double Alpha = sin(Omega) / (2.0 * Resonance);

		Coefficients Coeffs;
		Coeffs.b0 = 1.0;
		Coeffs.b1 = -2.0 * CosOmega;
		Coeffs.b2 = 1.0;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		// Normalize the coefficients
		Coeffs.b0 /= Coeffs.a0;
		Coeffs.b1 /= Coeffs.a0;
		Coeffs.b2 /= Coeffs.a0;
		Coeffs.a1 /= Coeffs.a0;
		Coeffs.a2 /= Coeffs.a0;

		Filter->SetCoefficients(&Coeffs);
	}

private:
	Stage *m_Stages;
};

#endif