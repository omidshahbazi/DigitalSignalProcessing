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
	enum class FilterDesign : uint8
	{
		Identical,
		Butterworth,
		LinkwitzRiley
	};

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
		const ValueType cos2Omega = Math::Cos(2.0 * omega);
		const ValueType sinOmega = Math::Sin(omega);
		const ValueType sin2Omega = Math::Sin(2.0 * omega);

		ValueType gainTotal = 1.0;

		for (uint8 i = 0; i < StageCount; ++i)
		{
#ifdef ARM_SIMD_BIQUAD
			const Coefficients &coeffs = m_Stage.Coeffs[i];
#else
			const Coefficients &coeffs = m_Stages[i].Coeffs;
#endif

			ValueType numReal = coeffs.b0 + coeffs.b1 * cosOmega + coeffs.b2 * cos2Omega;
			ValueType numImag = -(coeffs.b1 * sinOmega + coeffs.b2 * sin2Omega);

			ValueType denReal = 1.0 + coeffs.a1 * cosOmega + coeffs.a2 * cos2Omega;
			ValueType denImag = -(coeffs.a1 * sinOmega + coeffs.a2 * sin2Omega);

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

			Coeffs.b0 = 1.0;
			Coeffs.b1 = 0.0;
			Coeffs.b2 = 0.0;
			Coeffs.a1 = 0.0;
			Coeffs.a2 = 0.0;
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
		ASSERT(0 <= Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(QUALITY_FACTOR_MINIMUM <= Quality && Quality <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", Quality);

		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * Quality);

		Coefficients CoeffsArray[StageCount] = {};
		CoeffsArray[0].b0 = (1.0 - CosOmega) / 2.0;
		CoeffsArray[0].b1 = 1.0 - CosOmega;
		CoeffsArray[0].b2 = (1.0 - CosOmega) / 2.0;
		CoeffsArray[0].a1 = -2.0 * CosOmega;
		CoeffsArray[0].a2 = 1.0 - Alpha;

		Normalize(CoeffsArray[0], 1.0 + Alpha);
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
		ASSERT(0 <= Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(QUALITY_FACTOR_MINIMUM <= Quality && Quality <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", Quality);

		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType CosOmega = Math::Cos(Omega);
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * Quality);

		Coefficients CoeffsArray[StageCount] = {};
		CoeffsArray[0].b0 = (1.0 + CosOmega) / 2.0;
		CoeffsArray[0].b1 = -(1.0 + CosOmega);
		CoeffsArray[0].b2 = (1.0 + CosOmega) / 2.0;
		CoeffsArray[0].a1 = -2.0 * CosOmega;
		CoeffsArray[0].a2 = 1.0 - Alpha;

		Normalize(CoeffsArray[0], 1.0 + Alpha);
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
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * Quality);

		Coefficients CoeffsArray[StageCount] = {};
		CoeffsArray[0].b0 = Alpha;
		CoeffsArray[0].b1 = 0.0;
		CoeffsArray[0].b2 = -Alpha;
		CoeffsArray[0].a1 = -2.0 * CosOmega;
		CoeffsArray[0].a2 = 1.0 - Alpha;

		Normalize(CoeffsArray[0], 1.0 + Alpha);
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
		const ValueType Alpha = Math::Sin(Omega) / (2.0 * Quality);

		Coefficients CoeffsArray[StageCount] = {};
		CoeffsArray[0].b0 = 1.0;
		CoeffsArray[0].b1 = -2.0 * CosOmega;
		CoeffsArray[0].b2 = 1.0;
		CoeffsArray[0].a1 = -2.0 * CosOmega;
		CoeffsArray[0].a2 = 1.0 - Alpha;

		Normalize(CoeffsArray[0], 1.0 + Alpha);
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

		const ValueType A = Math::Power(10.0, Gain / 40.0);
		const ValueType Omega = Math::TWO_PI_VALUE * Center / SampleRate;
		const ValueType Sn = Math::Sin(Omega);
		const ValueType Cs = Math::Cos(Omega);
		const ValueType Alpha = Sn / (2.0 * Quality);

		Coefficients CoeffsArray[StageCount] = {};
		CoeffsArray[0].b0 = 1.0 + (Alpha * A);
		CoeffsArray[0].b1 = -2.0 * Cs;
		CoeffsArray[0].b2 = 1.0 - (Alpha * A);
		CoeffsArray[0].a1 = -2.0 * Cs;
		CoeffsArray[0].a2 = 1.0 - (Alpha / A);

		Normalize(CoeffsArray[0], 1.0 + (Alpha / A));
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
		ASSERT(0 <= Cutoff && Cutoff <= MAX_FREQUENCY, "Invalid Cutoff %f", Cutoff);
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		ASSERT(SLOPE_FACTOR_MINIMUM <= Slope && Slope <= SLOPE_FACTOR_MAXIMUM, "Invalid Slope %f", Slope);

		const ValueType A = Math::Power(10.0, Gain / 40.0);
		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType Sn = Math::Sin(Omega);
		const ValueType Cs = Math::Cos(Omega);

		const ValueType Alpha = Sn / 2.0 * Math::SquareRoot((A + 1.0 / A) * (1.0 / Slope - 1.0) + 2.0);
		const ValueType TwoSqrtAAlpha = 2.0 * Math::SquareRoot(A) * Alpha;

		Coefficients CoeffsArray[StageCount] = {};
		CoeffsArray[0].b0 = A * ((A + 1.0) - (A - 1.0) * Cs + TwoSqrtAAlpha);
		CoeffsArray[0].b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * Cs);
		CoeffsArray[0].b2 = A * ((A + 1.0) - (A - 1.0) * Cs - TwoSqrtAAlpha);
		CoeffsArray[0].a1 = -2.0 * ((A - 1.0) + (A + 1.0) * Cs);
		CoeffsArray[0].a2 = (A + 1.0) + (A - 1.0) * Cs - TwoSqrtAAlpha;

		Normalize(CoeffsArray[0], (A + 1.0) + (A - 1.0) * Cs + TwoSqrtAAlpha);
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

		const ValueType A = Math::Power(10.0, Gain / 40.0);
		const ValueType Omega = Math::TWO_PI_VALUE * Cutoff / SampleRate;
		const ValueType Sn = Math::Sin(Omega);
		const ValueType Cs = Math::Cos(Omega);

		const ValueType Alpha = Sn / 2.0 * Math::SquareRoot((A + 1.0 / A) * (1.0 / Slope - 1.0) + 2.0);
		const ValueType TwoSqrtAAlpha = 2.0 * Math::SquareRoot(A) * Alpha;

		Coefficients CoeffsArray[StageCount] = {};
		CoeffsArray[0].b0 = A * ((A + 1.0) + (A - 1.0) * Cs + TwoSqrtAAlpha);
		CoeffsArray[0].b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * Cs);
		CoeffsArray[0].b2 = A * ((A + 1.0) + (A - 1.0) * Cs - TwoSqrtAAlpha);
		CoeffsArray[0].a1 = 2.0 * ((A - 1.0) - (A + 1.0) * Cs);
		CoeffsArray[0].a2 = (A + 1.0) - (A - 1.0) * Cs - TwoSqrtAAlpha;

		Normalize(CoeffsArray[0], (A + 1.0) - (A - 1.0) * Cs + TwoSqrtAAlpha);
		Filter->SetCoefficients(CoeffsArray);
	}

private:
	static ValueType GetDistributedQualityFactor(uint8 StageIndex, QualityFactor Base, FilterDesign Design)
	{
		if constexpr (StageCount == 1)
			return Base;

		const float n = StageCount * 2;

		switch (Design)
		{
		case FilterDesign::Identical:
			return (ValueType)Base;

		case FilterDesign::Butterworth:
		{
			const float angle = (2 * StageIndex + 1) * Math::PI_VALUE / (2 * n);
			return 1.0 / (2.0 * Math::Sin(angle));
		}

		case FilterDesign::LinkwitzRiley:
		{
			const float angle = (2 * (StageIndex / 2) + 1) * Math::PI_VALUE / n;
			return 1.0 / (2.0 * Math::Sin(angle));
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

template <typename T, uint8 StageCount, uint32 SampleRate>
class BiquadBasedFilter : protected BiquadFilter<T, StageCount, SampleRate>
{
public:
	BiquadBasedFilter(void)
	{
		this->UpdateCoefficients();
	}

	void Process(T *Buffer, uint8 Count) override
	{
		return BiquadFilter<T, StageCount, SampleRate>::Process(Buffer, Count);
	}

	T Process(T Value) override
	{
		BiquadFilter<T, StageCount, SampleRate>::Process(&Value, 1);

		return Value;
	}

protected:
	virtual void UpdateCoefficients(void)
	{
	}
};

template <typename T, uint8 StageCount, uint32 SampleRate>
class BiquadBandBasedFilter : public BiquadBasedFilter<T, StageCount, SampleRate>
{
private:
	typedef BiquadBasedFilter<T, StageCount, SampleRate> Base;

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
	void SetAsBandPass(void)
	{
		BiquadFilter<T, StageCount, SampleRate>::SetBandPassCoefficients(this, m_Band);
	}
	void SetAsBandStop(void)
	{
		BiquadFilter<T, StageCount, SampleRate>::SetBandStopCoefficients(this, m_Band);
	}

	using Base::UpdateCoefficients;

protected:
	FrequencyBand m_Band;
};

template <typename T, uint8 StageCount, uint32 SampleRate>
class BiquadCutoffBasedFilter : public BiquadBasedFilter<T, StageCount, SampleRate>
{
private:
	typedef BiquadBasedFilter<T, StageCount, SampleRate> Base;

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

template <typename T, uint8 StageCount, uint32 SampleRate>
class BiquadCutoffPassBasedFilter : public BiquadCutoffBasedFilter<T, StageCount, SampleRate>
{
private:
	typedef BiquadCutoffBasedFilter<T, StageCount, SampleRate> Base;

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
		BiquadFilter<T, StageCount, SampleRate>::SetLowPassCoefficients(this, Base::m_Cutoff, m_Quality);
	}
	void SetAsHighPass(void)
	{
		BiquadFilter<T, StageCount, SampleRate>::SetHighPassCoefficients(this, Base::m_Cutoff, m_Quality);
	}

	using Base::UpdateCoefficients;

protected:
	QualityFactor m_Quality;
};

template <typename T, uint8 StageCount, uint32 SampleRate>
class BiquadCutoffShelfBasedFilter : public BiquadCutoffBasedFilter<T, StageCount, SampleRate>
{
private:
	typedef BiquadCutoffBasedFilter<T, StageCount, SampleRate> Base;

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
		BiquadFilter<T, StageCount, SampleRate>::SetLowShelfCoefficients(this, Base::m_Cutoff, m_Gain, m_Slope);
	}
	void SetAsHighShelf(void)
	{
		BiquadFilter<T, StageCount, SampleRate>::SetHighShelfCoefficients(this, Base::m_Cutoff, m_Gain, m_Slope);
	}

	using Base::UpdateCoefficients;

protected:
	dBGain m_Gain;
	SlopeFactor m_Slope;
};

#endif