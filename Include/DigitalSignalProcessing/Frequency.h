#pragma once
#ifndef FREQUENCY_H
#define FREQUENCY_H

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
void FixBandFrequency(Frequency& Low, Frequency& High);

struct LinearOctave
{
public:
	LinearOctave(void) = default;
	explicit LinearOctave(float Value)
		: m_Value(Value)
	{
	}
	LinearOctave(LogarithmicOctave Value);
	LinearOctave(Frequency Low, Frequency High);

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
	LogarithmicOctave(LinearOctave Value);
	LogarithmicOctave(Frequency Low, Frequency High);

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 0;
};

typedef LogarithmicOctave LogarithmicRatio;
typedef LinearOctave LinearRatio;

Frequency operator*(Frequency Frequency, LinearOctave Octave);

Frequency operator/(Frequency Frequency, LinearOctave Octave);

Frequency operator*(Frequency Frequency, LogarithmicOctave Octave);

Frequency operator/(Frequency Frequency, LogarithmicOctave Octave);

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
	FrequencyBand(Frequency Low, Frequency High);

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

	Frequency GetLow(void) const;

	Frequency GetHigh(void) const;

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

	QualityFactor(FrequencyBand Band);

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 0.707;
};

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

static const Frequency MIN_FREQUENCY = LOWEST_FREQUENCY;
static const Frequency MAX_FREQUENCY = HIGHEST_FREQUENCY;

#endif