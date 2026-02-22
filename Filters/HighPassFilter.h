#pragma once
#ifndef HIGH_PASS_FILTER_H
#define HIGH_PASS_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class HighPassFilter : private BiquadFilter<T, 1, SampleRate>
{
public:
	HighPassFilter(void)
		: m_CutoffFrequency(1),
		  m_QualityFactory(1)
	{
		SetCutoffFrequency(MIN_FREQUENCY);
		SetQualityFactory(QUALITY_FACTOR_MAXIMALLY_FLAT);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
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
	void SetQualityFactory(float Value)
	{
		ASSERT(QUALITY_FACTOR_MINIMUM <= Value && Value <= QUALITY_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_QualityFactory = Value;

		Update();
	}
	float GetQualityFactory(void) const
	{
		return m_QualityFactory;
	}

	T Process(T Value) override
	{
		return BiquadFilter<T, 1, SampleRate>::Process(Value);
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1, SampleRate>::SetHighPassFilterCoefficients(this, m_CutoffFrequency, m_QualityFactory);
	}

private:
	float m_CutoffFrequency;
	float m_QualityFactory;
};

#endif