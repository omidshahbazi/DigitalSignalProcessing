#pragma once
#ifndef FREQUENCY_H
#define FREQUENCY_H

#include "Math.h"
#include "Gain.h"

struct LogarithmicOctave;
struct QualityFactor;
struct SlopeFactor;

#define KHz *1'000.0

struct Frequency
{
public:
	Frequency(void) = default;
	explicit Frequency(float Value)
		: m_Value(Value)
	{
	}

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 0;
};

static const Frequency LOWEST_FREQUENCY(5);
static const Frequency HIGHEST_FREQUENCY(20 KHz);

// So we won't run into out of range QualityFactor values [0.01, 10]
static void FixBandFrequency(Frequency &Low, Frequency &High)
{
	// LowerBoundRatio = e^(2 * ASinH(1/20))
	const double LowerBoundRatio = 1.1053423;
	const double UpperBoundRatio = 10000;

	High = (Frequency)Math::Clamp(High, Low * LowerBoundRatio, Low * UpperBoundRatio);

	if (High > HIGHEST_FREQUENCY)
	{
		High = HIGHEST_FREQUENCY;

		Low = (Frequency)(High / LowerBoundRatio);
	}
}

struct LinearOctave
{
public:
	LinearOctave(void) = default;
	explicit LinearOctave(float Value)
		: m_Value(Value)
	{
	}
	LinearOctave(LogarithmicOctave Value);
	LinearOctave(Frequency Low, Frequency High)
		: m_Value(0)
	{
		FixBandFrequency(Low, High);

		m_Value = High / Low;
	}

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 1;
};

struct LogarithmicOctave
{
public:
	LogarithmicOctave(void) = default;
	explicit LogarithmicOctave(float Value)
		: m_Value(Value)
	{
	}
	LogarithmicOctave(LinearOctave Value)
		: m_Value(Math::Log2((float)Value))
	{
	}
	LogarithmicOctave(Frequency Low, Frequency High)
		: m_Value((float)(LogarithmicOctave)LinearOctave(Low, High))
	{
	}

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 0;
};

inline LinearOctave::LinearOctave(LogarithmicOctave Value)
	: m_Value(Math::Power2((float)Value))
{
}

typedef LogarithmicOctave LogarithmicRatio;
typedef LinearOctave LinearRatio;

inline Frequency operator*(Frequency Frequency, LinearOctave Octave)
{
	return ::Frequency((float)Frequency * (float)Octave);
}

inline Frequency operator/(Frequency Frequency, LinearOctave Octave)
{
	return ::Frequency((float)Frequency / (float)Octave);
}

inline Frequency operator*(Frequency Frequency, LogarithmicOctave Octave)
{
	return Frequency * LinearOctave(Octave);
}

inline Frequency operator/(Frequency Frequency, LogarithmicOctave Octave)
{
	return Frequency / LinearOctave(Octave);
}

struct FrequencyBand
{
public:
	FrequencyBand(void) = default;
	FrequencyBand(Frequency Center, LogarithmicOctave Bandwidth)
		: m_Center(Center),
		  m_Bandwidth(Bandwidth)
	{
	}
	FrequencyBand(Frequency Center, QualityFactor Quality);
	FrequencyBand(Frequency Low, Frequency High)
	{
		FixBandFrequency(Low, High);

		m_Center = (Frequency)Math::FrequencyLerp(Low, High, 0.5);
		m_Bandwidth = LogarithmicOctave(Low, High);
	}

	void SetCenter(Frequency Center)
	{
		m_Center = Center;
	}
	Frequency GetCenter(void) const
	{
		return m_Center;
	}

	void SetBandwidth(LogarithmicOctave Bandwidth)
	{
		m_Bandwidth = Bandwidth;
	}
	LogarithmicOctave GetBandwidth(void) const
	{
		return m_Bandwidth;
	}

	void SetQualityFactor(QualityFactor Quality);
	QualityFactor GetQualityFactor(void) const;

	Frequency GetLow(void) const
	{
		return (Frequency)Math::Max(LOWEST_FREQUENCY, m_Center / LinearOctave(LogarithmicOctave(m_Bandwidth * 0.5)));
	}

	Frequency GetHigh(void) const
	{
		return (Frequency)Math::Min(HIGHEST_FREQUENCY, m_Center * LinearOctave(LogarithmicOctave(m_Bandwidth * 0.5)));
	}

	operator QualityFactor(void) const;

private:
	Frequency m_Center;
	LogarithmicOctave m_Bandwidth;
};

struct QualityFactor
{
public:
	QualityFactor(void) = default;
	explicit QualityFactor(float Value)
		: m_Value(Value)
	{
	}

	QualityFactor(FrequencyBand Band)
	{
		m_Value = 1 / (2 * Math::SinH(Math::HALF_LOG_NATURAL_2 * Band.GetBandwidth()));
	}

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 0.707;
};

inline FrequencyBand::FrequencyBand(Frequency Center, QualityFactor Quality)
	: m_Center(Center)
{
	SetQualityFactor(Quality);
}

inline void FrequencyBand::SetQualityFactor(QualityFactor Quality)
{
	m_Bandwidth = LogarithmicOctave(2 * Math::ASinh(1 / (2 * Quality)) / Math::LOG_NATURAL_2);
}

inline QualityFactor FrequencyBand::GetQualityFactor(void) const
{
	return QualityFactor(*this);
}

inline FrequencyBand::operator QualityFactor(void) const
{
	return GetQualityFactor();
}

struct SlopeFactor
{
public:
	SlopeFactor(void) = default;
	explicit SlopeFactor(float Value)
		: m_Value(Value)
	{
	}

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 1;
};

#endif