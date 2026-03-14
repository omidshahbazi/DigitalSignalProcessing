#pragma once
#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"

static constexpr float QUALITY_FACTOR_MINIMUM = 0.01;
static constexpr float QUALITY_FACTOR_CRITICAL_DAMPING = 0.5;
static constexpr float QUALITY_FACTOR_MAXIMALLY_FLAT = 0.707;
static constexpr float QUALITY_FACTOR_RESONANCE = 1.5;
static constexpr float QUALITY_FACTOR_PEAK = 2;
static constexpr float QUALITY_FACTOR_HIGH_SELECTIVE = 5;
static constexpr float QUALITY_FACTOR_MAXIMUM = 100;

static constexpr float SLOPE_FACTOR_MINIMUM = 0.01;
static constexpr float SLOPE_FACTOR_GENTLE = 0.5;
static constexpr float SLOPE_FACTOR_MAXIMALLY_FLAT = 1.0;
static constexpr float SLOPE_FACTOR_STEEP = 1.5;
static constexpr float SLOPE_FACTOR_RESONANT = 2.0;
static constexpr float SLOPE_FACTOR_MAXIMUM = 5.0;

template <typename T, uint8_t StageCount, uint32 SampleRateValue>
class BiquadFilter : public Filter<T, SampleRateValue>
{
	static_assert(StageCount != 0, "StageCount cannot be 0");
	static_assert(StageCount < 2, "StageCount more than 1 is not implemented");

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
		: m_Stages{}
	{
	}

	void SetCoefficients(const Coefficients Values[StageCount])
	{
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			m_Stages[i].Coeffs = Values[i];

			Normalize(m_Stages[i].Coeffs);
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
	// CutoffFrequency [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a smoother transition near the cutoff frequency, reducing peak resonance but also making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the cutoff frequency, which can lead to oscillations or instability.
	static void SetLowPassFilterCoefficients(BiquadFilter *Filter, float CutoffFrequency, float QualityFactor = QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		SetLowPassFilterCoefficients(Filter, SampleRateValue, CutoffFrequency, QualityFactor);
	}
	// CutoffFrequency [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a smoother transition near the cutoff frequency, reducing peak resonance but also making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the cutoff frequency, which can lead to oscillations or instability.
	static void SetLowPassFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, float QualityFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);
		ASSERT(Filter != nullptr, "Filter cannot be null");

		double Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		double CosOmega = Math::Cos(Omega);
		double Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = (1.0 - CosOmega) / 2.0;
		Coeffs.b1 = 1.0 - CosOmega;
		Coeffs.b2 = (1.0 - CosOmega) / 2.0;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Filter->SetCoefficients(CoeffsArray);
	}

	// CutoffFrequency [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a smoother transition near the cutoff frequency, reducing peak resonance but also making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the cutoff frequency, which can lead to oscillations or instability.
	static void SetHighPassFilterCoefficients(BiquadFilter *Filter, float CutoffFrequency, float QualityFactor = QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		SetHighPassFilterCoefficients(Filter, SampleRateValue, CutoffFrequency, QualityFactor);
	}
	// CutoffFrequency [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a smoother transition near the cutoff frequency, reducing peak resonance but also making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the cutoff frequency, which can lead to oscillations or instability.
	static void SetHighPassFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, float QualityFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);

		double Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		double CosOmega = Math::Cos(Omega);
		double Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = (1.0 + CosOmega) / 2.0;
		Coeffs.b1 = -(1.0 + CosOmega);
		Coeffs.b2 = (1.0 + CosOmega) / 2.0;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Filter->SetCoefficients(CoeffsArray);
	}

	// CenterFrequency [1, MAX_FREQUENCY]
	// Bandwidth [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader bandwidth and less peak resonance, making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the center frequency, which can lead to more pronounced resonance or oscillations.
	static void SetBandPassFilterCoefficients(BiquadFilter *Filter, float CenterFrequency, float Bandwidth, float QualityFactor = QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		SetBandPassFilterCoefficients(Filter, SampleRateValue, CenterFrequency, Bandwidth, QualityFactor);
	}

	// CenterFrequency [1, MAX_FREQUENCY]
	// Bandwidth [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader bandwidth and less peak resonance, making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the center frequency, which can lead to more pronounced resonance or oscillations.
	static void SetBandPassFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CenterFrequency, float Bandwidth, float QualityFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CenterFrequency && CenterFrequency <= MAX_FREQUENCY, "Invalid CenterFrequency %f", CenterFrequency);
		ASSERT(0 <= Bandwidth && Bandwidth <= MAX_FREQUENCY, "Invalid Bandwidth %f", Bandwidth);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);

		double Omega = Math::TWO_PI_VALUE * CenterFrequency / SampleRate;
		double CosOmega = Math::Cos(Omega);
		double Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = Alpha;
		Coeffs.b1 = 0.0;
		Coeffs.b2 = -Alpha;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Filter->SetCoefficients(CoeffsArray);
	}

	// CenterFrequency [1, MAX_FREQUENCY]
	// Bandwidth [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetBandStopFilterCoefficients(BiquadFilter *Filter, float CenterFrequency, float Bandwidth, float QualityFactor = QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		SetBandStopFilterCoefficients(Filter, SampleRateValue, CenterFrequency, Bandwidth, QualityFactor);
	}
	// CenterFrequency [1, MAX_FREQUENCY]
	// Bandwidth [1, MAX_FREQUENCY]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetBandStopFilterCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CenterFrequency, float Bandwidth, float QualityFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CenterFrequency && CenterFrequency <= MAX_FREQUENCY, "Invalid CenterFrequency %f", CenterFrequency);
		ASSERT(0 <= Bandwidth && Bandwidth <= MAX_FREQUENCY, "Invalid Bandwidth %f", Bandwidth);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);

		double Omega = Math::TWO_PI_VALUE * CenterFrequency / SampleRate;
		double CosOmega = Math::Cos(Omega);
		double Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = 1.0;
		Coeffs.b1 = -2.0 * CosOmega;
		Coeffs.b2 = 1.0;
		Coeffs.a0 = 1.0 + Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Filter->SetCoefficients(CoeffsArray);
	}

	// CutoffFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetPeakEQCoefficients(BiquadFilter *Filter, float CutoffFrequency, float Gain, float QualityFactor = QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		SetPeakEQCoefficients(Filter, SampleRateValue, CutoffFrequency, Gain, QualityFactor);
	}
	// CutoffFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetPeakEQCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, float Gain, float QualityFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);

		double A = Math::Power(10.0, Gain / 40.0);
		double Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		double Sn = Math::Sin(Omega);
		double Cs = Math::Cos(Omega);
		double Alpha = Sn / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = 1.0 + (Alpha * A);
		Coeffs.b1 = -2.0 * Cs;
		Coeffs.b2 = 1.0 - (Alpha * A);
		Coeffs.a0 = 1.0 + (Alpha / A);
		Coeffs.a1 = -2.0 * Cs;
		Coeffs.a2 = 1.0 - (Alpha / A);

		Filter->SetCoefficients(CoeffsArray);
	}

	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetLowShelfCoefficients(BiquadFilter *Filter, float CutoffFrequency, float Gain, float SlopeFactor = SLOPE_FACTOR_MAXIMALLY_FLAT)
	{
		SetLowShelfCoefficients(Filter, SampleRateValue, CutoffFrequency, Gain, SlopeFactor);
	}
	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetLowShelfCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, float Gain, float SlopeFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= SlopeFactor && SlopeFactor <= SLOPE_FACTOR_MAXIMUM, "Invalid SlopeFactor %f", SlopeFactor);

		double A = Math::Power(10.0, Gain / 40.0);
		double Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		double Sn = Math::Sin(Omega);
		double Cs = Math::Cos(Omega);
		// S = Shelf Slope. When S=1, the shelf is as steep as possible without ringing.
		double Alpha = (Sn / 2.0) * Math::SquareRoot((A + 1.0 / A) * (1.0 / SlopeFactor - 1.0) + 2.0);
		double TwoSqrtAAlpha = 2.0 * Math::SquareRoot(A) * Alpha;

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = A * ((A + 1.0) - (A - 1.0) * Cs + TwoSqrtAAlpha);
		Coeffs.b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * Cs);
		Coeffs.b2 = A * ((A + 1.0) - (A - 1.0) * Cs - TwoSqrtAAlpha);
		Coeffs.a0 = (A + 1.0) + (A - 1.0) * Cs + TwoSqrtAAlpha;
		Coeffs.a1 = -2.0 * ((A - 1.0) + (A + 1.0) * Cs);
		Coeffs.a2 = (A + 1.0) + (A - 1.0) * Cs - TwoSqrtAAlpha;

		Filter->SetCoefficients(CoeffsArray);
	}

	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetHighShelfCoefficients(BiquadFilter *Filter, float CutoffFrequency, float Gain, float SlopeFactor = SLOPE_FACTOR_MAXIMALLY_FLAT)
	{
		SetHighShelfCoefficients(Filter, SampleRateValue, CutoffFrequency, Gain, SlopeFactor);
	}
	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetHighShelfCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, float Gain, float SlopeFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= SlopeFactor && SlopeFactor <= SLOPE_FACTOR_MAXIMUM, "Invalid SlopeFactor %f", SlopeFactor);

		double A = Math::Power(10.0, Gain / 40.0);
		double Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		double Sn = Math::Sin(Omega);
		double Cs = Math::Cos(Omega);
		double Alpha = (Sn / 2.0) * Math::SquareRoot((A + 1.0 / A) * (1.0 / SlopeFactor - 1.0) + 2.0);
		double TwoSqrtAAlpha = 2.0 * Math::SquareRoot(A) * Alpha;

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = A * ((A + 1.0) + (A - 1.0) * Cs + TwoSqrtAAlpha);
		Coeffs.b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * Cs);
		Coeffs.b2 = A * ((A + 1.0) + (A - 1.0) * Cs - TwoSqrtAAlpha);
		Coeffs.a0 = (A + 1.0) - (A - 1.0) * Cs + TwoSqrtAAlpha;
		Coeffs.a1 = 2.0 * ((A - 1.0) - (A + 1.0) * Cs);
		Coeffs.a2 = (A + 1.0) - (A - 1.0) * Cs - TwoSqrtAAlpha;

		Filter->SetCoefficients(CoeffsArray);
	}

private:
	static void Normalize(Coefficients &Coeffs)
	{
		if (Math::Absolute(Coeffs.a0) < 1e-9)
			return;

		Coeffs.b0 /= Coeffs.a0;
		Coeffs.b1 /= Coeffs.a0;
		Coeffs.b2 /= Coeffs.a0;
		Coeffs.a1 /= Coeffs.a0;
		Coeffs.a2 /= Coeffs.a0;
		Coeffs.a0 = 1.0;
	}

private:
	Stage m_Stages[StageCount];
};

#endif