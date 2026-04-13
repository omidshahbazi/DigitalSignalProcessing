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

static const QualityFactor QUALITY_FACTOR_MINIMUM(0.01); // Has to be in sync with the result of FixBandFrequency()
static const QualityFactor QUALITY_FACTOR_CRITICAL_DAMPING(0.5);
static const QualityFactor QUALITY_FACTOR_MAXIMALLY_FLAT(0.707);
static const QualityFactor QUALITY_FACTOR_RESONANCE(1.5);
static const QualityFactor QUALITY_FACTOR_PEAK(2);
static const QualityFactor QUALITY_FACTOR_HIGH_SELECTIVE(5);
static const QualityFactor QUALITY_FACTOR_MAXIMUM(10); // Has to be in sync with the result of FixBandFrequency()

static const SlopeFactor SLOPE_FACTOR_MINIMUM(0.01);
static const SlopeFactor SLOPE_FACTOR_GENTLE(0.5);
static const SlopeFactor SLOPE_FACTOR_MAXIMALLY_FLAT(1);
static const SlopeFactor SLOPE_FACTOR_STEEP(1.5);
static const SlopeFactor SLOPE_FACTOR_RESONANT(2);
static const SlopeFactor SLOPE_FACTOR_MAXIMUM(2.3);

static const LogarithmicOctave OCTAVE_MINIMUM(0.1442);
static const LogarithmicOctave OCTAVE_NORMAL(1.41);
static const LogarithmicOctave OCTAVE_MAXIMUM(10);

enum class BiquadFilterDesign : uint8
{
	Identical = 0,
	Butterworth,
	LinkwitzRiley
};

template <typename T, uint32 SampleRateValue, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BiquadFilter : public Filter<T, SampleRateValue>
{
	static_assert(StageCount != 0, "StageCount cannot be 0");

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
	BiquadFilter(void)
#ifdef ARM_SIMD_BIQUAD
		: m_Stage{}
#else
		: m_Stages{}
#endif
	{
		Reset();

		SetBypassCoefficients(this);

#ifdef ARM_SIMD_BIQUAD
		arm_biquad_cascade_df1_init_f32(&m_Stage.Instance, StageCount, &m_Stage.Coeffs[0].b0, m_Stage.State);
#endif
	}

	void SetCoefficients(const Coefficients Values[StageCount])
	{
#ifdef ARM_SIMD_BIQUAD
		for (uint8 i = 0; i < StageCount; ++i)
			m_Stage.Coeffs[i] = Values[i];
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
			ValueType sample = Buffer[i];

			for (uint8 j = 0; j < StageCount; ++j)
			{
				Stage &stage = m_Stages[j];

				ValueType output = (sample * stage.Coeffs.b0) + stage.z1;

				stage.z1 = (sample * stage.Coeffs.b1) + (output * stage.Coeffs.a1) + stage.z2;
				stage.z2 = (sample * stage.Coeffs.b2) + (output * stage.Coeffs.a2);

				sample = output;
			}

			Buffer[i] = sample;
		}
#endif
	}

	LinearGain GetGainAt(float Frequency) const
	{
		const ValueType omega = Math::TWO_PI_VALUE * Frequency / SampleRateValue;
		const ValueType cosOmega = Math::Cos(omega);
		const ValueType cos2Omega = Math::Cos(2 * omega);
		const ValueType sinOmega = Math::Sin(omega);
		const ValueType sin2Omega = Math::Sin(2 * omega);

		ValueType gainTotal = 1;

		for (uint8 i = 0; i < StageCount; ++i)
		{
#ifdef ARM_SIMD_BIQUAD
			const Coefficients &coeffs = m_Stage.Coeffs[i];
#else
			const Coefficients &coeffs = m_Stages[i].Coeffs;
#endif

			ValueType numReal = coeffs.b0 + coeffs.b1 * cosOmega + coeffs.b2 * cos2Omega;
			ValueType numImag = -(coeffs.b1 * sinOmega + coeffs.b2 * sin2Omega);

			ValueType denReal = 1 - coeffs.a1 * cosOmega - coeffs.a2 * cos2Omega;
			ValueType denImag = -(-coeffs.a1 * sinOmega - coeffs.a2 * sin2Omega);

			ValueType powerNum = numReal * numReal + numImag * numImag;
			ValueType powerDen = denReal * denReal + denImag * denImag;

			gainTotal *= powerNum / powerDen;
		}

		return LinearGain(Math::SquareRoot(gainTotal));
	}

public:
	// Resets the filter to a bypass state (Identity).
	static void SetBypassCoefficients(BiquadFilter *Filter)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");

		Coefficients CoeffsArray[StageCount] = {};

		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			Coeffs.b0 = 1;
			Coeffs.b1 = 0;
			Coeffs.b2 = 0;
			Coeffs.a1 = 0;
			Coeffs.a2 = 0;
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetAllPassCoefficients(BiquadFilter *Filter, FrequencyBand Band)
	{
		SetAllPassCoefficients(Filter, SampleRateValue, Band);
	}
	// SampleRate: (0, ...]
	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetAllPassCoefficients(BiquadFilter *Filter, uint32 SampleRate, FrequencyBand Band)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Band.Center %f", Band.GetCenter());
		ASSERT(QUALITY_FACTOR_MINIMUM < Band.GetQualityFactor() && Band.GetQualityFactor() <= QUALITY_FACTOR_MAXIMUM, "Invalid Band.QualityFactor %f", Band.GetQualityFactor());

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band);

		const ValueType Omega = Math::TWO_PI_VALUE * (float)Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = 1 - Alpha;
			Coeffs.b1 = -2 * CosOmega;
			Coeffs.b2 = 1 + Alpha;
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Cutoff: (0, MAX_FREQUENCY]
	// Quality: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetLowPassCoefficients(BiquadFilter *Filter, Frequency Cutoff, QualityFactor Quality)
	{
		SetLowPassCoefficients(Filter, SampleRateValue, Cutoff, Quality);
	}
	// SampleRate: (0, ...]
	// Cutoff: (0, MAX_FREQUENCY]
	// Quality: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetLowPassCoefficients(BiquadFilter *Filter, uint32 SampleRate, Frequency Cutoff, QualityFactor Quality)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(QUALITY_FACTOR_MINIMUM <= Quality && Quality <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", Quality);

		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType bBase = (1 - CosOmega) / 2;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = bBase;
			Coeffs.b1 = 1 - CosOmega;
			Coeffs.b2 = bBase;
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Cutoff: (0, MAX_FREQUENCY]
	// Quality: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetHighPassCoefficients(BiquadFilter *Filter, Frequency Cutoff, QualityFactor Quality)
	{
		SetHighPassCoefficients(Filter, SampleRateValue, Cutoff, Quality);
	}
	// SampleRate: (0, ...]
	// Cutoff: (0, MAX_FREQUENCY]
	// Quality: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetHighPassCoefficients(BiquadFilter *Filter, uint32 SampleRate, Frequency Cutoff, QualityFactor Quality)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(QUALITY_FACTOR_MINIMUM <= Quality && Quality <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", Quality);

		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType bCommon = (1 + CosOmega) / 2;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = bCommon;
			Coeffs.b1 = -(1 + CosOmega);
			Coeffs.b2 = bCommon;
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetBandPassCoefficients(BiquadFilter *Filter, FrequencyBand Band)
	{
		SetBandPassCoefficients(Filter, SampleRateValue, Band);
	}
	// SampleRate: (0, ...]
	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetBandPassCoefficients(BiquadFilter *Filter, uint32 SampleRate, FrequencyBand Band)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Band.Center %f", Band.GetCenter());
		ASSERT(QUALITY_FACTOR_MINIMUM < Band.GetQualityFactor() && Band.GetQualityFactor() <= QUALITY_FACTOR_MAXIMUM, "Invalid Band.QualityFactor %f", Band.GetQualityFactor());

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band);

		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = Alpha;
			Coeffs.b1 = 0;
			Coeffs.b2 = -Alpha;
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetBandStopCoefficients(BiquadFilter *Filter, FrequencyBand Band)
	{
		SetBandStopCoefficients(Filter, SampleRateValue, Band);
	}
	// SampleRate: (0, ...]
	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetBandStopCoefficients(BiquadFilter *Filter, uint32 SampleRate, FrequencyBand Band)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Band.Center %f", Band.GetCenter());
		ASSERT(QUALITY_FACTOR_MINIMUM < Band.GetQualityFactor() && Band.GetQualityFactor() <= QUALITY_FACTOR_MAXIMUM, "Invalid Band.QualityFactor %f", Band.GetQualityFactor());

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band);

		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType b1 = -2 * CosOmega;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = 1;
			Coeffs.b1 = b1;
			Coeffs.b2 = 1;
			Coeffs.a1 = b1;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetPeakResonatorCoefficients(BiquadFilter *Filter, FrequencyBand Band)
	{
		SetPeakResonatorCoefficients(Filter, SampleRateValue, Band);
	}
	// SampleRate: (0, ...]
	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	static void SetPeakResonatorCoefficients(BiquadFilter *Filter, uint32 SampleRate, FrequencyBand Band)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Band.Center %f", Band.GetCenter());
		ASSERT(QUALITY_FACTOR_MINIMUM < Band.GetQualityFactor() && Band.GetQualityFactor() <= QUALITY_FACTOR_MAXIMUM, "Invalid Band.QualityFactor %f", Band.GetQualityFactor());

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band);

		const ValueType Omega = Math::TWO_PI_VALUE * (float)Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = Alpha;
			Coeffs.b1 = 0;
			Coeffs.b2 = -Alpha;
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Cutoff: (0, MAX_FREQUENCY]
	// Gain: [-20dB, 20dB] Boost or cut below Cutoff frequency.
	// Slope: [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM]
	static void SetLowShelfCoefficients(BiquadFilter *Filter, Frequency Cutoff, dBGain Gain, SlopeFactor Slope)
	{
		SetLowShelfCoefficients(Filter, SampleRateValue, Cutoff, Gain, Slope);
	}
	// SampleRate: (0, ...]
	// Cutoff: (0, MAX_FREQUENCY]
	// Gain: [-20dB, 20dB] Boost or cut below Cutoff frequency.
	// Slope: [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM]
	static void SetLowShelfCoefficients(BiquadFilter *Filter, uint32 SampleRate, Frequency Cutoff, dBGain Gain, SlopeFactor Slope)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= Slope && Slope <= SLOPE_FACTOR_MAXIMUM, "Invalid Slope %f", Slope);

		const ValueType A = Math::Power(10.0, Gain / 40);
		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType Ap1 = A + 1;
		const ValueType Am1 = A - 1;
		const ValueType Am1Cos = Am1 * CosOmega;
		const ValueType Ap1Cos = Ap1 * CosOmega;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType Alpha = (SinOmega / 2) * Math::SquareRoot((A + 1 / A) * (1 / (float)Slope - 1) + 2);
			const ValueType TwoSqrtAAlpha = 2 * Math::SquareRoot(A) * Alpha;

			Coeffs.b0 = A * (Ap1 - Am1Cos + TwoSqrtAAlpha);
			Coeffs.b1 = 2 * A * (Am1 - Ap1Cos);
			Coeffs.b2 = A * (Ap1 - Am1Cos - TwoSqrtAAlpha);
			Coeffs.a1 = -2 * (Am1 + Ap1Cos);
			Coeffs.a2 = Ap1 + Am1Cos - TwoSqrtAAlpha;

			Normalize(Coeffs, Ap1 + Am1Cos + TwoSqrtAAlpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Cutoff: (0, MAX_FREQUENCY]
	// Gain: [-20dB, 20dB] Boost or cut above Cutoff frequency.
	// Slope: [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM]
	static void SetHighShelfCoefficients(BiquadFilter *Filter, Frequency Cutoff, dBGain Gain, SlopeFactor Slope)
	{
		SetHighShelfCoefficients(Filter, SampleRateValue, Cutoff, Gain, Slope);
	}
	// SampleRate: (0, ...]
	// Cutoff: (0, MAX_FREQUENCY]
	// Gain: [-20dB, 20dB] Boost or cut above Cutoff frequency.
	// Slope: [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM]
	static void SetHighShelfCoefficients(BiquadFilter *Filter, uint32 SampleRate, Frequency Cutoff, dBGain Gain, SlopeFactor Slope)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= Slope && Slope <= SLOPE_FACTOR_MAXIMUM, "Invalid Slope %f", Slope);

		const ValueType A = Math::Power(10.0, Gain / 40);
		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType Ap1 = A + 1;
		const ValueType Am1 = A - 1;
		const ValueType Am1Cos = Am1 * CosOmega;
		const ValueType Ap1Cos = Ap1 * CosOmega;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType Alpha = (SinOmega / 2) * Math::SquareRoot((A + 1 / A) * (1 / (float)Slope - 1) + 2);
			const ValueType TwoSqrtAAlpha = 2 * Math::SquareRoot(A) * Alpha;

			Coeffs.b0 = A * (Ap1 + Am1Cos + TwoSqrtAAlpha);
			Coeffs.b1 = -2 * A * (Am1 + Ap1Cos);
			Coeffs.b2 = A * (Ap1 + Am1Cos - TwoSqrtAAlpha);
			Coeffs.a1 = 2 * (Am1 - Ap1Cos);
			Coeffs.a2 = Ap1 - Am1Cos - TwoSqrtAAlpha;

			Normalize(Coeffs, Ap1 - Am1Cos + TwoSqrtAAlpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	// Gain: [-20dB, 20dB] Boost or cut at Center frequency.
	static void SetPeakEQCoefficients(BiquadFilter *Filter, FrequencyBand Band, dBGain Gain)
	{
		SetPeakEQCoefficients(Filter, SampleRateValue, Band, Gain);
	}
	// SampleRate: (0, ...]
	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	// Gain: [-20dB, 20dB] Boost or cut at Center frequency.
	static void SetPeakEQCoefficients(BiquadFilter *Filter, uint32 SampleRate, FrequencyBand Band, dBGain Gain)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Band.Center %f", Band.GetCenter());
		ASSERT(QUALITY_FACTOR_MINIMUM < Band.GetQualityFactor() && Band.GetQualityFactor() <= QUALITY_FACTOR_MAXIMUM, "Invalid Band.QualityFactor %f", Band.GetQualityFactor());
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band);

		const ValueType A = Math::Power(10.0, Gain / 40);
		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = 1 + (Alpha * A);
			Coeffs.b1 = -2 * CosOmega;
			Coeffs.b2 = 1 - (Alpha * A);
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - (Alpha / A);

			Normalize(Coeffs, 1 + (Alpha / A));
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	// Gain: [-20dB, 20dB] Boost or cut below Cutoff frequency.
	static void SetTiltEQCoefficients(BiquadFilter *Filter, FrequencyBand Band, dBGain Gain)
	{
		SetTiltEQCoefficients(Filter, SampleRateValue, Band, Gain);
	}
	// SampleRate: (0, ...]
	// Band.Center: (0, MAX_FREQUENCY]
	// Band.QualityFactor: [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	// Gain: [-20dB, 20dB] Boost or cut below Cutoff frequency.
	static void SetTiltEQCoefficients(BiquadFilter *Filter, uint32 SampleRate, FrequencyBand Band, dBGain Gain)
	{
		ASSERT(Filter != nullptr, "Filter cannot be null");
		ASSERT(0 < SampleRate, "Invalid SampleRate %u", SampleRate);
		ASSERT(0 < Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Band.Center %f", Band.GetCenter());
		ASSERT(QUALITY_FACTOR_MINIMUM < Band.GetQualityFactor() && Band.GetQualityFactor() <= QUALITY_FACTOR_MAXIMUM, "Invalid Band.QualityFactor %f", Band.GetQualityFactor());
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band);

		const ValueType A = Math::Power(10.0, Gain / 40);
		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8 i = 0; i < StageCount; ++i)
		{
			Coefficients &Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);
			const ValueType TwoSqrtAAlpha = 2 * Math::SquareRoot(A) * Alpha;

			Coeffs.b0 = A * ((A + 1) + (A - 1) * CosOmega + TwoSqrtAAlpha);
			Coeffs.b1 = -2 * A * ((A - 1) + (A + 1) * CosOmega);
			Coeffs.b2 = A * ((A + 1) + (A - 1) * CosOmega - TwoSqrtAAlpha);
			Coeffs.a1 = 2 * ((A - 1) - (A + 1) * CosOmega);
			Coeffs.a2 = (A + 1) - (A - 1) * CosOmega - TwoSqrtAAlpha;

			Normalize(Coeffs, (A + 1.0) - (A - 1.0) * CosOmega + TwoSqrtAAlpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

private:
	static ValueType GetDistributedQualityFactor(uint8 StageIndex, QualityFactor Base)
	{
		if constexpr (StageCount == 1)
			return Base;

		const float n = StageCount * 2;

		switch (Design)
		{
		case BiquadFilterDesign::Identical:
			return (ValueType)Base;

		case BiquadFilterDesign::Butterworth:
		{
			const float angle = (2 * StageIndex + 1) * Math::PI_VALUE / (2 * n);
			return 1 / (2 * Math::Sin(angle));
		}

		case BiquadFilterDesign::LinkwitzRiley:
		{
			const float angle = (2 * (StageIndex / 2) + 1) * Math::PI_VALUE / n;
			return 1 / (2 * Math::Sin(angle));
		}
		}

		return 0;
	}

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

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BiquadBasedFilter : protected BiquadFilter<T, SampleRate, StageCount, Design>
{
public:
	BiquadBasedFilter(void)
	{
		this->UpdateCoefficients();
	}

	// void Process(T *Buffer, uint8 Count) override
	// {
	// 	return BiquadFilter<T, SampleRate, StageCount, Design>::Process(Buffer, Count);
	// }

	// T Process(T Value) override
	// {
	// 	BiquadFilter<T, SampleRate, StageCount, Design>::Process(&Value, 1);

	// 	return Value;
	// }

	using Filter<T, SampleRate>::Process;

protected:
	virtual void UpdateCoefficients(void)
	{
	}
};

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BiquadBandBasedFilter : public BiquadBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadBandBasedFilter(void)
		: m_Band(MIN_FREQUENCY, OCTAVE_NORMAL)
	{
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	void SetCenterFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_Band.SetCenter(Value);

		this->UpdateCoefficients();
	}
	Frequency GetCenterFrequency(void) const
	{
		return m_Band.GetCenter();
	}

	// [OCTAVE_MINIMUM, OCTAVE_MINIMUM]
	void SetBandwidth(LogarithmicOctave Value)
	{
		ASSERT(OCTAVE_MINIMUM <= Value && Value <= OCTAVE_MAXIMUM, "Invalid Value %f", Value);

		m_Band.SetBandwidth(Value);

		this->UpdateCoefficients();
	}
	LogarithmicOctave GetBandwidth(void) const
	{
		return m_Band.GetBandwidth();
	}

	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetQualityFactor(QualityFactor Value)
	{
		ASSERT(QUALITY_FACTOR_MINIMUM <= Value && Value <= QUALITY_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_Band.SetQualityFactor(Value);

		this->UpdateCoefficients();
	}
	QualityFactor GetQualityFactor(void) const
	{
		return m_Band;
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBand(FrequencyBand Band)
	{
		ASSERT(MIN_FREQUENCY <= Band.GetLow() && Band.GetLow() <= MAX_FREQUENCY, "Invalid Band.Low %f", Band.GetLow());
		ASSERT(MIN_FREQUENCY <= Band.GetHigh() && Band.GetHigh() <= MAX_FREQUENCY, "Invalid Band.High %f", Band.GetHigh());

		m_Band = Band;

		this->UpdateCoefficients();
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBand(Frequency Low, Frequency High)
	{
		SetBand(FrequencyBand(Low, High));
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [OCTAVE_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(Frequency Center, LogarithmicOctave Bandwidth)
	{
		ASSERT(MIN_FREQUENCY <= Center && Center <= MAX_FREQUENCY, "Invalid Center %f", Center);
		ASSERT(OCTAVE_MINIMUM <= Bandwidth && Bandwidth <= OCTAVE_MAXIMUM, "Invalid Bandwidth %f", Bandwidth);

		m_Band.SetCenter(Center);
		m_Band.SetBandwidth(Bandwidth);

		this->UpdateCoefficients();
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(Frequency Center, QualityFactor Quality)
	{
		ASSERT(MIN_FREQUENCY <= Center && Center <= MAX_FREQUENCY, "Invalid Center %f", Center);
		ASSERT(QUALITY_FACTOR_MINIMUM <= Quality && Quality <= QUALITY_FACTOR_MAXIMUM, "Invalid Quality %f", Quality);

		m_Band.SetCenter(Center);
		m_Band.SetQualityFactor(Quality);

		this->UpdateCoefficients();
	}

protected:
	void SetAsAllPass(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetAllPassCoefficients(this, m_Band);
	}
	void SetAsBandPass(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetBandPassCoefficients(this, m_Band);
	}
	void SetAsBandStop(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetBandStopCoefficients(this, m_Band);
	}
	void SetAsPeakResonator(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetPeakResonatorCoefficients(this, m_Band);
	}

	using Base::UpdateCoefficients;

protected:
	FrequencyBand m_Band;
};

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BiquadCutoffBasedFilter : public BiquadBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadCutoffBasedFilter(void)
		: m_Cutoff(MIN_FREQUENCY)
	{
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	void SetCutoffFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_Cutoff = Value;

		this->UpdateCoefficients();
	}
	Frequency GetCutoffFrequency(void) const
	{
		return m_Cutoff;
	}

protected:
	using Base::UpdateCoefficients;

protected:
	Frequency m_Cutoff;
};

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BiquadCutoffPassBasedFilter : public BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadCutoffPassBasedFilter(void)
		: m_Quality(QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
	}

	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetQualityFactor(QualityFactor Value)
	{
		ASSERT(QUALITY_FACTOR_MINIMUM <= Value && Value <= QUALITY_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_Quality = Value;

		this->UpdateCoefficients();
	}
	QualityFactor GetQualityFactor(void) const
	{
		return m_Quality;
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(Frequency Cutoff, QualityFactor Quality)
	{
		ASSERT(MIN_FREQUENCY <= Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(QUALITY_FACTOR_MINIMUM <= Quality && Quality <= QUALITY_FACTOR_MAXIMUM, "Invalid Quality %f", Quality);

		Base::m_Cutoff = Cutoff;
		m_Quality = Quality;

		this->UpdateCoefficients();
	}

protected:
	void SetAsLowPass(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetLowPassCoefficients(this, Base::m_Cutoff, m_Quality);
	}
	void SetAsHighPass(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetHighPassCoefficients(this, Base::m_Cutoff, m_Quality);
	}

	using Base::UpdateCoefficients;

protected:
	QualityFactor m_Quality;
};

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BiquadCutoffShelfBasedFilter : public BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadCutoffShelfBasedFilter(void)
		: m_Gain(NORMAL_GAIN),
		  m_Slope(SLOPE_FACTOR_MAXIMALLY_FLAT)
	{
	}

	// [-20dB, 20dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_Gain = Value;

		this->UpdateCoefficients();
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	// [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM]
	void SetSlopeFactor(SlopeFactor Value)
	{
		ASSERT(SLOPE_FACTOR_MINIMUM <= Value && Value <= SLOPE_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_Slope = Value;

		this->UpdateCoefficients();
	}
	SlopeFactor GetSlopeFactor(void) const
	{
		return m_Slope;
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [-20dB, 20dB]
	// [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM]
	void SetParameters(Frequency Cutoff, dBGain Gain, SlopeFactor Slope)
	{
		ASSERT(MIN_FREQUENCY <= Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= Slope && Slope <= SLOPE_FACTOR_MAXIMUM, "Invalid Slope %f", Slope);

		Base::m_Cutoff = Cutoff;
		m_Gain = Gain;
		m_Slope = Slope;

		this->UpdateCoefficients();
	}

protected:
	void SetAsLowShelf(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetLowShelfCoefficients(this, Base::m_Cutoff, m_Gain, m_Slope);
	}
	void SetAsHighShelf(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetHighShelfCoefficients(this, Base::m_Cutoff, m_Gain, m_Slope);
	}

	using Base::UpdateCoefficients;

protected:
	dBGain m_Gain;
	SlopeFactor m_Slope;
};

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BiquadEQBasedFilter : public BiquadBandBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBandBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	// [-20dB, 20dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_Gain = Value;

		UpdateCoefficients();
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [-20dB, 20dB]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(FrequencyBand Band, dBGain Gain)
	{
		ASSERT(MIN_FREQUENCY <= Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Cutoff %f", Band.GetCenter());
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);

		Base::m_Band = Band;
		m_Gain = Gain;

		UpdateCoefficients();
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [OCTAVE_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(Frequency Center, LogarithmicOctave Bandwidth, dBGain Gain)
	{
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);

		m_Gain = Gain;
		Base::SetParameters(Center, Bandwidth);
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(Frequency Center, QualityFactor Quality, dBGain Gain)
	{
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);

		m_Gain = Gain;
		Base::SetParameters(Center, Quality);
	}

protected:
	void SetAsPeakEQ(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetPeakEQCoefficients(this, Base::m_Band, m_Gain);
	}
	void SetAsTiltEQ(void)
	{
		BiquadFilter<T, SampleRate, StageCount, Design>::SetTiltEQCoefficients(this, Base::m_Band, m_Gain);
	}

	using Base::UpdateCoefficients;

protected:
	dBGain m_Gain;
};

#endif