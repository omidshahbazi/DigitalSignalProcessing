#pragma once
#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Frequency.h"
#include "DigitalSignalProcessing/Gain.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/NotNull.h"
#include "DigitalSignalProcessing/Debug.h"

//Reference:
// https://www.w3.org/TR/audio-eq-cookbook
// https://ccrma.stanford.edu/~jos/filters

#ifdef ARM_SIMD_BIQUAD
#include <CMSIS-DSP/Include/arm_math.h>

static constexpr uint8_t ARM_SIMD_BIQUAD_STATE_COUNT = 4;

#undef HIGH_RESOLUTION_BIQUAD
#endif

static constexpr QualityFactor QUALITY_FACTOR_MINIMUM(0.01); // Has to be in sync with the result of FixBandFrequency()
static constexpr QualityFactor QUALITY_FACTOR_CRITICAL_DAMPING(0.5);
static constexpr QualityFactor QUALITY_FACTOR_MAXIMALLY_FLAT(0.707);
static constexpr QualityFactor QUALITY_FACTOR_RESONANCE(1.5);
static constexpr QualityFactor QUALITY_FACTOR_PEAK(2);
static constexpr QualityFactor QUALITY_FACTOR_HIGH_SELECTIVE(5);
static constexpr QualityFactor QUALITY_FACTOR_MAXIMUM(10); // Has to be in sync with the result of FixBandFrequency()

static constexpr SlopeFactor SLOPE_FACTOR_MINIMUM(0.01);
static constexpr SlopeFactor SLOPE_FACTOR_GENTLE(0.5);
static constexpr SlopeFactor SLOPE_FACTOR_MAXIMALLY_FLAT(1);
static constexpr SlopeFactor SLOPE_FACTOR_STEEP(1.5);
static constexpr SlopeFactor SLOPE_FACTOR_RESONANT(2);
static constexpr SlopeFactor SLOPE_FACTOR_MAXIMUM(2.3);

static constexpr LogarithmicOctave OCTAVE_MINIMUM(0.1442);
static constexpr LogarithmicOctave OCTAVE_NORMAL(1.41);
static constexpr LogarithmicOctave OCTAVE_MAXIMUM(10);

#define CHECK_CUTOFF_QUALITY_VALUE() \
	ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Quality, QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM);

#define CHECK_CUTOFF_SLOPE_VALUE() \
	ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Slope, SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM);

#define CHECK_BAND_VALUES() \
	ASSERT_VALUE_RANGE_MAX_INCLUSIVE(Band.GetCenter(), 0, MAX_FREQUENCY); \
	ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Band.GetQualityFactor(), QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM);

enum class BiquadFilterDesigns : uint8_t
{
	Identical = 0,
	Butterworth,
	LinkwitzRiley
};

template <typename T, uint32_t SampleRateValue, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BiquadFilter : public Filter<T, SampleRateValue>
{
	STATIC_ASSERT(StageCount != 0, "StageCount cannot be 0");

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
		for (uint8_t i = 0; i < StageCount; ++i)
			m_Stage.Coeffs[i] = Values[i];
#else
		for (uint8_t i = 0; i < StageCount; ++i)
			m_Stages[i].Coeffs = Values[i];
#endif
	}

	void Reset(void)
	{
#ifdef ARM_SIMD_BIQUAD
		Memory::Set(m_Stage.State, 0, ARM_SIMD_BIQUAD_STATE_COUNT * StageCount);
#else
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			m_Stages[i].z1 = 0;
			m_Stages[i].z2 = 0;
		}
#endif
	}

	void Process(T* Buffer, uint8_t Count) override
	{
#ifdef ARM_SIMD_BIQUAD
		arm_biquad_cascade_df1_f32(&m_Stage.Instance, Buffer, Buffer, Count);
#else
		for (uint8_t i = 0; i < Count; ++i)
		{
			ValueType sample = Buffer[i];

			for (uint8_t j = 0; j < StageCount; ++j)
			{
				Stage& stage = m_Stages[j];

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

		for (uint8_t i = 0; i < StageCount; ++i)
		{
#ifdef ARM_SIMD_BIQUAD
			const Coefficients& coeffs = m_Stage.Coeffs[i];
#else
			const Coefficients& coeffs = m_Stages[i].Coeffs;
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
	static void SetBypassCoefficients(NotNull<BiquadFilter> Filter)
	{
		Coefficients CoeffsArray[StageCount] = {};

		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

			Coeffs.b0 = 1;
			Coeffs.b1 = 0;
			Coeffs.b2 = 0;
			Coeffs.a1 = 0;
			Coeffs.a2 = 0;
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	static void SetAllPassCoefficients(NotNull<BiquadFilter> Filter, FrequencyBand Band)
	{
		SetAllPassCoefficients(Filter, SampleRateValue, Band);
	}
	static void SetAllPassCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, FrequencyBand Band)
	{
		CHECK_BAND_VALUES();

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band, SampleRate);

		const ValueType Omega = Math::TWO_PI_VALUE * (float)Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

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

	static void SetLowPassCoefficients(NotNull<BiquadFilter> Filter, Frequency_full Cutoff, QualityFactor Quality)
	{
		SetLowPassCoefficients(Filter, SampleRateValue, Cutoff, Quality);
	}
	static void SetLowPassCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, Frequency_full Cutoff, QualityFactor Quality)
	{
		CHECK_CUTOFF_QUALITY_VALUE();

		const ValueType Omega = Math::TWO_PI_VALUE * (Frequency)Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType OneMinusCos = 2 * Math::Sin(Omega / 2) * Math::Sin(Omega / 2);
		const ValueType bBase = OneMinusCos / 2;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = bBase;
			Coeffs.b1 = OneMinusCos;
			Coeffs.b2 = bBase;
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	static void SetHighPassCoefficients(NotNull<BiquadFilter> Filter, Frequency_full Cutoff, QualityFactor Quality)
	{
		SetHighPassCoefficients(Filter, SampleRateValue, Cutoff, Quality);
	}
	static void SetHighPassCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, Frequency_full Cutoff, QualityFactor Quality)
	{
		CHECK_CUTOFF_QUALITY_VALUE();

		const ValueType Omega = Math::TWO_PI_VALUE * (Frequency)Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType bCommon = (1 + CosOmega) / 2;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

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

	static void SetBandPassCoefficients(NotNull<BiquadFilter> Filter, FrequencyBand Band)
	{
		SetBandPassCoefficients(Filter, SampleRateValue, Band);
	}
	static void SetBandPassCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, FrequencyBand Band)
	{
		CHECK_BAND_VALUES();

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band, SampleRate);

		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

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

	static void SetBandStopCoefficients(NotNull<BiquadFilter> Filter, FrequencyBand Band)
	{
		SetBandStopCoefficients(Filter, SampleRateValue, Band);
	}
	static void SetBandStopCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, FrequencyBand Band)
	{
		CHECK_BAND_VALUES();

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band, SampleRate);

		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType b1 = -2 * CosOmega;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

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

	static void SetPeakResonatorCoefficients(NotNull<BiquadFilter> Filter, FrequencyBand Band)
	{
		SetPeakResonatorCoefficients(Filter, SampleRateValue, Band);
	}
	static void SetPeakResonatorCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, FrequencyBand Band)
	{
		CHECK_BAND_VALUES();

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band, SampleRate);

		const ValueType Omega = Math::TWO_PI_VALUE * (float)Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType StageGain = Math::Power((ValueType)Quality, (ValueType)1 / StageCount);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);

			Coeffs.b0 = Alpha * StageGain;
			Coeffs.b1 = 0;
			Coeffs.b2 = -Alpha * StageGain;
			Coeffs.a1 = -2 * CosOmega;
			Coeffs.a2 = 1 - Alpha;

			Normalize(Coeffs, 1 + Alpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

	static void SetLowShelfCoefficients(NotNull<BiquadFilter> Filter, Frequency_full Cutoff, dBGain_20 Gain, SlopeFactor Slope)
	{
		SetLowShelfCoefficients(Filter, SampleRateValue, Cutoff, Gain, Slope);
	}
	static void SetLowShelfCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, Frequency_full Cutoff, dBGain_20 Gain, SlopeFactor Slope)
	{
		CHECK_CUTOFF_SLOPE_VALUE();

		const ValueType A = Math::Power(10.0, (dBGain)Gain / (40 * StageCount));
		const ValueType Omega = Math::TWO_PI_VALUE * (Frequency)Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType Ap1 = A + 1;
		const ValueType Am1 = A - 1;
		const ValueType Am1Cos = Am1 * CosOmega;
		const ValueType Ap1Cos = Ap1 * CosOmega;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

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

	static void SetHighShelfCoefficients(NotNull<BiquadFilter> Filter, Frequency_full Cutoff, dBGain_20 Gain, SlopeFactor Slope)
	{
		SetHighShelfCoefficients(Filter, SampleRateValue, Cutoff, Gain, Slope);
	}
	static void SetHighShelfCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, Frequency_full Cutoff, dBGain_20 Gain, SlopeFactor Slope)
	{
		CHECK_CUTOFF_SLOPE_VALUE();

		const ValueType A = Math::Power(10.0, (dBGain)Gain / (40 * StageCount));
		const ValueType Omega = Math::TWO_PI_VALUE * (Frequency)Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		const ValueType Ap1 = A + 1;
		const ValueType Am1 = A - 1;
		const ValueType Am1Cos = Am1 * CosOmega;
		const ValueType Ap1Cos = Ap1 * CosOmega;

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

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

	static void SetPeakEQCoefficients(NotNull<BiquadFilter> Filter, FrequencyBand Band, dBGain_20 Gain)
	{
		SetPeakEQCoefficients(Filter, SampleRateValue, Band, Gain);
	}
	static void SetPeakEQCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, FrequencyBand Band, dBGain_20 Gain)
	{
		CHECK_BAND_VALUES();

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band, SampleRate);

		const ValueType A = Math::Power(10.0, (dBGain)Gain / (40 * StageCount));
		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

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

	static void SetTiltEQCoefficients(NotNull<BiquadFilter> Filter, FrequencyBand Band, dBGain_20 Gain)
	{
		SetTiltEQCoefficients(Filter, SampleRateValue, Band, Gain);
	}
	static void SetTiltEQCoefficients(NotNull<BiquadFilter> Filter, puint32_t SampleRate, FrequencyBand Band, dBGain_20 Gain)
	{
		CHECK_BAND_VALUES();

		const Frequency Center = Band.GetCenter();
		const QualityFactor Quality(Band, SampleRate);

		const ValueType A = Math::Power(10.0, (dBGain)Gain / (40 * StageCount));
		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType SinOmega = Math::Sin(Omega);

		Coefficients CoeffsArray[StageCount] = {};
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			Coefficients& Coeffs = CoeffsArray[i];

			const ValueType DistributedQ = GetDistributedQualityFactor(i, Quality);
			const ValueType Alpha = SinOmega / (2 * DistributedQ);
			const ValueType TwoSqrtAAlpha = 2 * Math::SquareRoot(A) * Alpha;

			const ValueType Comp = 1 / A;

			Coeffs.b0 = A * ((A + 1) + (A - 1) * CosOmega + TwoSqrtAAlpha) * Comp;
			Coeffs.b1 = -2 * A * ((A - 1) + (A + 1) * CosOmega) * Comp;
			Coeffs.b2 = A * ((A + 1) + (A - 1) * CosOmega - TwoSqrtAAlpha) * Comp;
			Coeffs.a1 = 2 * ((A - 1) - (A + 1) * CosOmega);
			Coeffs.a2 = (A + 1) - (A - 1) * CosOmega - TwoSqrtAAlpha;

			Normalize(Coeffs, (A + 1.0) - (A - 1.0) * CosOmega + TwoSqrtAAlpha);
		}

		Filter->SetCoefficients(CoeffsArray);
	}

private:
	static ValueType GetDistributedQualityFactor(uint8_t StageIndex, QualityFactor Base)
	{
		if constexpr (StageCount == 1)
			return Base;

		const float n = StageCount * 2;

		if constexpr (Design == BiquadFilterDesigns::Identical)
			return (ValueType)Base;
		else if constexpr (Design == BiquadFilterDesigns::Butterworth)
		{
			const float angle = (2 * StageIndex + 1) * Math::PI_VALUE / (2 * n);
			return 1 / (2 * Math::Sin(angle));
		}
		else if constexpr (Design == BiquadFilterDesigns::LinkwitzRiley)
		{
			const float angle = (2 * (StageIndex / 2) + 1) * Math::PI_VALUE / n;
			return 1 / (2 * Math::Sin(angle));
		}

		NOT_IMPLEMENTED();

		return 0;
	}

	static void Normalize(Coefficients& Coeffs, ValueType a0)
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

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BiquadBasedFilter : protected BiquadFilter<T, SampleRate, StageCount, Design>
{
public:
	BiquadBasedFilter(void)
	{
		this->UpdateCoefficients();
	}

	FILTER_FORWARD_DECLARATION()

protected:
	virtual void UpdateCoefficients(void)
	{}
};

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BiquadBandBasedFilter : public BiquadBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadBandBasedFilter(void)
		: m_Band(MIN_FREQUENCY, OCTAVE_NORMAL)
	{}

	void SetCenterFrequency(Frequency_hearable Value)
	{
		m_Band.SetCenter(Value);

		this->UpdateCoefficients();
	}
	Frequency GetCenterFrequency(void) const
	{
		return m_Band.GetCenter();
	}

	void SetBandwidth(LogarithmicOctave Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, OCTAVE_MINIMUM, OCTAVE_MAXIMUM);

		m_Band.SetBandwidth(Value);

		this->UpdateCoefficients();
	}
	LogarithmicOctave GetBandwidth(void) const
	{
		return m_Band.GetBandwidth();
	}

	void SetQualityFactor(QualityFactor Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM);

		m_Band.SetQualityFactor(Value);

		this->UpdateCoefficients();
	}
	QualityFactor GetQualityFactor(void) const
	{
		return m_Band;
	}

	void SetBand(FrequencyBand Band)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Band.GetLow(), MIN_FREQUENCY, MAX_FREQUENCY);
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Band.GetHigh(), MIN_FREQUENCY, MAX_FREQUENCY);

		m_Band = Band;

		this->UpdateCoefficients();
	}

	void SetBand(Frequency_hearable Low, Frequency_hearable High)
	{
		SetBand(FrequencyBand(Low, High));
	}

	void SetParameters(Frequency_hearable Center, LogarithmicOctave Bandwidth)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Bandwidth, OCTAVE_MINIMUM, OCTAVE_MAXIMUM);

		m_Band.SetCenter(Center);
		m_Band.SetBandwidth(Bandwidth);

		this->UpdateCoefficients();
	}

	void SetParameters(Frequency_hearable Center, QualityFactor Quality)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Quality, QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM);

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

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BiquadCutoffBasedFilter : public BiquadBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadCutoffBasedFilter(void)
		: m_Cutoff(MIN_FREQUENCY)
	{}

	void SetCutoffFrequency(Frequency_hearable Value)
	{
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

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BiquadCutoffPassBasedFilter : public BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadCutoffPassBasedFilter(void)
		: m_Quality(QUALITY_FACTOR_MAXIMALLY_FLAT)
	{}

	void SetQualityFactor(QualityFactor Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM);

		m_Quality = Value;

		this->UpdateCoefficients();
	}
	QualityFactor GetQualityFactor(void) const
	{
		return m_Quality;
	}

	void SetParameters(Frequency_hearable Cutoff, QualityFactor Quality)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Quality, QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM);

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

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BiquadCutoffShelfBasedFilter : public BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	BiquadCutoffShelfBasedFilter(void)
		: m_Gain(NORMAL_GAIN),
		m_Slope(SLOPE_FACTOR_MAXIMALLY_FLAT)
	{}

	void SetGain(dBGain_20 Value)
	{
		m_Gain = Value;

		this->UpdateCoefficients();
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	void SetSlopeFactor(SlopeFactor Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM);

		m_Slope = Value;

		this->UpdateCoefficients();
	}
	SlopeFactor GetSlopeFactor(void) const
	{
		return m_Slope;
	}

	void SetParameters(Frequency_hearable Cutoff, dBGain_20 Gain, SlopeFactor Slope)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Slope, SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM);

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

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BiquadEQBasedFilter : public BiquadBandBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBandBasedFilter<T, SampleRate, StageCount, Design> Base;

public:
	void SetGain(dBGain_20 Value)
	{
		m_Gain = Value;

		UpdateCoefficients();
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	void SetParameters(FrequencyBand Band, dBGain_20 Gain)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Band.GetCenter(), MIN_FREQUENCY, MAX_FREQUENCY);

		Base::m_Band = Band;
		m_Gain = Gain;

		UpdateCoefficients();
	}

	void SetParameters(Frequency_hearable Center, LogarithmicOctave Bandwidth, dBGain_20 Gain)
	{
		m_Gain = Gain;
		Base::SetParameters(Center, Bandwidth);
	}

	void SetParameters(Frequency_hearable Center, QualityFactor Quality, dBGain_20 Gain)
	{
		m_Gain = Gain;
		Base::SetParameters(Center, Quality);
	}

	using Base::SetParameters;

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

#undef CHECK_CUTOFF_QUALITY_VALUE
#undef CHECK_CUTOFF_SLOPE_VALUE
#undef CHECK_BAND_VALUES

#endif