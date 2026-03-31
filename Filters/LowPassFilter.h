#pragma once
#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class LowPassFilter : private BiquadFilter<T, 1, SampleRate>
{
public:
	LowPassFilter(void)
		: m_CutoffFrequency(MIN_FREQUENCY),
		  m_QualityFactor(QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		Update();
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	void SetCutoffFrequency(float Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_CutoffFrequency = Value;

		Update();
	}
	float GetCutoffFrequency(void) const
	{
		return m_CutoffFrequency;
	}

	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetQualityFactor(float Value)
	{
		ASSERT(QUALITY_FACTOR_MINIMUM <= Value && Value <= QUALITY_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_QualityFactor = Value;

		Update();
	}
	float GetQualityFactor(void) const
	{
		return m_QualityFactor;
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(float CutoffFrequency, float QualityFactor)
	{
		ASSERT(MIN_FREQUENCY <= CutoffFrequency && CutoffFrequency <= MAX_FREQUENCY, "Invalid CutoffFrequency %f", CutoffFrequency);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);
		
		m_CutoffFrequency = CutoffFrequency;
		m_QualityFactor = QualityFactor;

		Update();
	}

	void Process(T *Buffer, uint8 Count) override
	{
		return BiquadFilter<T, 1, SampleRate>::Process(Buffer, Count);
	}

	T Process(T Value) override
	{
		BiquadFilter<T, 1, SampleRate>::Process(&Value, 1);

		return Value;
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1, SampleRate>::SetLowPassFilterCoefficients(this, m_CutoffFrequency, m_QualityFactor);
	}

private:
	float m_CutoffFrequency;
	float m_QualityFactor;
};

#endif