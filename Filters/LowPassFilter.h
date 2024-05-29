#pragma once
#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class LowPassFilter : private BiquadFilter<T, 1>
{
public:
	LowPassFilter(void)
		: m_CutoffFrequency(1),
		  m_Resonance(1)
	{
		SetCutoffFrequency(MAX_FREQUENCY);
		SetResonance(1);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetCutoffFrequency(float Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value");

		m_CutoffFrequency = Value;

		Update();
	}
	float GetCutoffFrequency(void) const
	{
		return m_CutoffFrequency;
	}

	//(0, 4000]
	void SetResonance(float Value)
	{
		ASSERT(0 < Value && Value <= 4000, "Invalid Value");

		m_Resonance = Value;

		Update();
	}
	float GetResonance(void) const
	{
		return m_Resonance;
	}

	T Process(T Value) override
	{
		return BiquadFilter<T, 1>::Process(Value);
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1>::SetLowPassFilterCoefficients(this, SampleRate, m_CutoffFrequency, m_Resonance);
	}

private:
	float m_CutoffFrequency;
	float m_Resonance;
};

#endif