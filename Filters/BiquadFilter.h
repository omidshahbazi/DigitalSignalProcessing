#pragma once
#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"

#ifdef ARM_SIMD_BIQUAD
#define ARM_MATH_LOOPUNROLL
#include "arm_math.h"

static constexpr uint8 ARM_SIMD_BIQUAD_STATE_COUNT = 4;

#undef HIGH_RESOLUTION_BIQUAD
#endif

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

template <typename T, uint8 StageCount, uint32 SampleRateValue>
class BiquadFilter : public Filter<T, SampleRateValue>
{
	static_assert(StageCount != 0, "StageCount cannot be 0");
	static_assert(StageCount < 2, "StageCount more than 1 is not implemented");

#ifdef ARM_SIMD_BIQUAD
	ASSERT_ON_ONLY_FLOAT_TYPE(T);
#endif

private:
#ifdef HIGH_RESOLUTION_BIQUAD
	typedef double ValueType;
#else
	typedef float ValueType;
#endif

public:
	struct Coefficients
	{
	public:
		ValueType b0;
		ValueType b1;
		ValueType b2;
		ValueType a1;
		ValueType a2;
	};

private:
	struct Stage
	{
	public:
#ifdef ARM_SIMD_BIQUAD
		arm_biquad_casd_df1_inst_f32 Instance;
		Coefficients Coeffs[StageCount];
		ValueType State[ARM_SIMD_BIQUAD_STATE_COUNT * StageCount];
#else
		Coefficients Coeffs;
		ValueType z1;
		ValueType z2;
#endif
	};

public:
	BiquadFilter()
#ifdef ARM_SIMD_BIQUAD
		: m_Stage{}
#else
		: m_Stages{}
#endif
	{
	}

	void SetCoefficients(const Coefficients Values[StageCount])
	{
#ifdef ARM_SIMD_BIQUAD
		for (uint8 i = 0; i < StageCount; ++i)
			m_Stage.Coeffs[i] = Values[i];

		arm_biquad_cascade_df1_init_f32(&m_Stage.Instance, StageCount, &m_Stage.Coeffs[0].b0, m_Stage.State);
#else
		for (uint8 i = 0; i < StageCount; ++i)
			m_Stages[i].Coeffs = Values[i];
#endif
	}

	void Reset(void)
	{
#ifdef ARM_SIMD_BIQUAD
		Memory::Set(m_Stage.State, 0, ARM_SIMD_BIQUAD_STATE_COUNT * StageCount);
#else
		for (uint8 i = 0; i < StageCount; ++i)
		{
			m_Stages[i].z1 = 0;
			m_Stages[i].z2 = 0;
		}
#endif
	}

	void Process(T *Buffer, uint8 Count) override
	{
#ifdef ARM_SIMD_BIQUAD
		arm_biquad_cascade_df1_f32(&m_Stage.Instance, Buffer, Buffer, Count);
#else
		for (uint8 i = 0; i < Count; ++i)
		{
			ValueType input = Buffer[i];

			for (uint8 j = 0; j < StageCount; ++j)
			{
				Stage &stage = m_Stages[j];

				ValueType output = (input * stage.Coeffs.b0) + stage.z1;

				stage.z1 = (input * stage.Coeffs.b1) + (output * stage.Coeffs.a1) + stage.z2;
				stage.z2 = (input * stage.Coeffs.b2) + (output * stage.Coeffs.a2);

				Buffer[i] = output;

				input = output;
			}
		}
#endif
	}

	LinearGain GetGainAt(float Frequency) const
	{
#ifdef ARM_SIMD_BIQUAD
#else
		const ValueType omega = Math::TWO_PI_VALUE * Frequency / SampleRateValue;
		const ValueType cosOmega = Math::Cos(omega);
		const ValueType cos2Omega = Math::Cos(2.0 * omega);
		const ValueType sinOmega = Math::Sin(omega);
		const ValueType sin2Omega = Math::Sin(2.0 * omega);

		ValueType gainTotal = 1.0;

		for (uint8 i = 0; i < StageCount; ++i)
		{
			const Stage &stage = m_Stages[i];

			ValueType a1Original = -stage.Coeffs.a1;
			ValueType a2Original = -stage.Coeffs.a2;

			ValueType numReal = stage.Coeffs.b0 + stage.Coeffs.b1 * cosOmega + stage.Coeffs.b2 * cos2Omega;
			ValueType numImag = -(stage.Coeffs.b1 * sinOmega + stage.Coeffs.b2 * sin2Omega);

			ValueType denReal = 1.0 + a1Original * cosOmega + a2Original * cos2Omega;
			ValueType denImag = -(a1Original * sinOmega + a2Original * sin2Omega);

			ValueType powerNum = numReal * numReal + numImag * numImag;
			ValueType powerDen = denReal * denReal + denImag * denImag;

			gainTotal *= Math::SquareRoot(powerNum / powerDen);
		}

		return gainTotal;
#endif
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

		const ValueType Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = (1.0 - CosOmega) / 2.0;
		Coeffs.b1 = 1.0 - CosOmega;
		Coeffs.b2 = (1.0 - CosOmega) / 2.0;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Normalize(Coeffs, 1.0 + Alpha);

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

		const ValueType Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = (1.0 + CosOmega) / 2.0;
		Coeffs.b1 = -(1.0 + CosOmega);
		Coeffs.b2 = (1.0 + CosOmega) / 2.0;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Normalize(Coeffs, 1.0 + Alpha);

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

		const ValueType Omega = Math::TWO_PI_VALUE * CenterFrequency / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = Alpha;
		Coeffs.b1 = 0.0;
		Coeffs.b2 = -Alpha;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Normalize(Coeffs, 1.0 + Alpha);

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

		const ValueType Omega = Math::TWO_PI_VALUE * CenterFrequency / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = 1.0;
		Coeffs.b1 = -2.0 * CosOmega;
		Coeffs.b2 = 1.0;
		Coeffs.a1 = -2.0 * CosOmega;
		Coeffs.a2 = 1.0 - Alpha;

		Normalize(Coeffs, 1.0 + Alpha);

		Filter->SetCoefficients(CoeffsArray);
	}

	// CutoffFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetPeakEQCoefficients(BiquadFilter *Filter, float CutoffFrequency, dBGain Gain, float QualityFactor = QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		SetPeakEQCoefficients(Filter, SampleRateValue, CutoffFrequency, Gain, QualityFactor);
	}
	// CutoffFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// QualityFactor [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM] SmoorthKneeSlope=[<QUALITY_FACTOR_MAXIMALLY_FLAT] Butterworth=[QUALITY_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>QUALITY_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetPeakEQCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, dBGain Gain, float QualityFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);

		const ValueType A = Math::Power(10.0, Gain / 40.0);
		const ValueType Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		const ValueType Sn = Math::Sin(Omega);
		const ValueType Cs = Math::Cos(Omega);
		const ValueType Alpha = Sn / (2.0 * QualityFactor);

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = 1.0 + (Alpha * A);
		Coeffs.b1 = -2.0 * Cs;
		Coeffs.b2 = 1.0 - (Alpha * A);
		Coeffs.a1 = -2.0 * Cs;
		Coeffs.a2 = 1.0 - (Alpha / A);

		Normalize(Coeffs, 1.0 + Alpha);

		Filter->SetCoefficients(CoeffsArray);
	}

	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetLowShelfCoefficients(BiquadFilter *Filter, float CutoffFrequency, dBGain Gain, float SlopeFactor = SLOPE_FACTOR_MAXIMALLY_FLAT)
	{
		SetLowShelfCoefficients(Filter, SampleRateValue, CutoffFrequency, Gain, SlopeFactor);
	}
	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetLowShelfCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, dBGain Gain, float SlopeFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= SlopeFactor && SlopeFactor <= SLOPE_FACTOR_MAXIMUM, "Invalid SlopeFactor %f", SlopeFactor);

		const ValueType A = Math::Power(10.0, Gain / 40.0);
		const ValueType Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		const ValueType Sn = Math::Sin(Omega);
		const ValueType Cs = Math::Cos(Omega);
		// S = Shelf Slope. When S=1, the shelf is as steep as possible without ringing.
		const ValueType Alpha = (Sn / 2.0) * Math::SquareRoot((A + 1.0 / A) * (1.0 / SlopeFactor - 1.0) + 2.0);
		const ValueType TwoSqrtAAlpha = 2.0 * Math::SquareRoot(A) * Alpha;

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = A * ((A + 1.0) - (A - 1.0) * Cs + TwoSqrtAAlpha);
		Coeffs.b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * Cs);
		Coeffs.b2 = A * ((A + 1.0) - (A - 1.0) * Cs - TwoSqrtAAlpha);
		Coeffs.a1 = -2.0 * ((A - 1.0) + (A + 1.0) * Cs);
		Coeffs.a2 = (A + 1.0) + (A - 1.0) * Cs - TwoSqrtAAlpha;

		Normalize(Coeffs, (A + 1.0) + (A - 1.0) * Cs + TwoSqrtAAlpha);

		Filter->SetCoefficients(CoeffsArray);
	}

	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetHighShelfCoefficients(BiquadFilter *Filter, float CutoffFrequency, dBGain Gain, float SlopeFactor = SLOPE_FACTOR_MAXIMALLY_FLAT)
	{
		SetHighShelfCoefficients(Filter, SampleRateValue, CutoffFrequency, Gain, SlopeFactor);
	}
	// CenterFrequency [1, MAX_FREQUENCY]
	// Gain [-20dB, 20dB]
	// SlopeFactor [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM] SmoorthKneeSlope=[<SLOPE_FACTOR_MAXIMALLY_FLAT] Butterworth=[SLOPE_FACTOR_MAXIMALLY_FLAT] SharpKneeSlope=[>SLOPE_FACTOR_MAXIMALLY_FLAT]
	// - Values less than 0.7 result in a broader notch and less resonance suppression, making the filter less effective at removing specific frequencies.
	// - Values greater than 1 increase resonance, creating a sharper notch around the center frequency, which can lead to more pronounced resonance or instability.
	static void SetHighShelfCoefficients(BiquadFilter *Filter, uint32 SampleRate, float CutoffFrequency, dBGain Gain, float SlopeFactor)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %f", SampleRate);
		ASSERT(0 < CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= SlopeFactor && SlopeFactor <= SLOPE_FACTOR_MAXIMUM, "Invalid SlopeFactor %f", SlopeFactor);

		const ValueType A = Math::Power(10.0, Gain / 40.0);
		const ValueType Omega = Math::TWO_PI_VALUE * CutoffFrequency / SampleRate;
		const ValueType Sn = Math::Sin(Omega);
		const ValueType Cs = Math::Cos(Omega);
		const ValueType Alpha = (Sn / 2.0) * Math::SquareRoot((A + 1.0 / A) * (1.0 / SlopeFactor - 1.0) + 2.0);
		const ValueType TwoSqrtAAlpha = 2.0 * Math::SquareRoot(A) * Alpha;

		Coefficients CoeffsArray[StageCount] = {};
		Coefficients &Coeffs = CoeffsArray[0];
		Coeffs.b0 = A * ((A + 1.0) + (A - 1.0) * Cs + TwoSqrtAAlpha);
		Coeffs.b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * Cs);
		Coeffs.b2 = A * ((A + 1.0) + (A - 1.0) * Cs - TwoSqrtAAlpha);
		Coeffs.a1 = 2.0 * ((A - 1.0) - (A + 1.0) * Cs);
		Coeffs.a2 = (A + 1.0) - (A - 1.0) * Cs - TwoSqrtAAlpha;

		Normalize(Coeffs, (A + 1.0) - (A - 1.0) * Cs + TwoSqrtAAlpha);

		Filter->SetCoefficients(CoeffsArray);
	}

private:
	static void Normalize(Coefficients &Coeffs, ValueType a0)
	{
		if (Math::Absolute(a0) < Math::EPSILON)
			return;

		Coeffs.b0 /= a0;
		Coeffs.b1 /= a0;
		Coeffs.b2 /= a0;
		Coeffs.a1 /= a0;
		Coeffs.a2 /= a0;

		Coeffs.a1 *= -1;
		Coeffs.a2 *= -1;
	}

private:
#ifdef ARM_SIMD_BIQUAD
	Stage m_Stage;
#else
	Stage m_Stages[StageCount];
#endif
};

#endif