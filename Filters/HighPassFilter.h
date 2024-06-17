#pragma once
#ifndef HIGH_PASS_FILTER_H
#define HIGH_PASS_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class HighPassFilter : private BiquadFilter<T, 1>
{
public:
	HighPassFilter(void)
		: m_CutoffFrequency(1),
		  m_Resonance(1)
	{
		SetCutoffFrequency(MIN_FREQUENCY);
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

	// [0.1, 10] Common [0.7, 1]
	// - Values less than 0.7 result in a smoother transition near the cutoff frequency, reducing peak resonance but also making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the cutoff frequency, which can lead to oscillations or instability.
	void SetResonance(float Value)
	{
		ASSERT(0.1 <= Value && Value <= 10, "Invalid Value");

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
		BiquadFilter<T, 1>::SetHighPassFilterCoefficients(this, SampleRate, m_CutoffFrequency, m_Resonance);
	}

private:
	float m_CutoffFrequency;
	float m_Resonance;
};

#endif