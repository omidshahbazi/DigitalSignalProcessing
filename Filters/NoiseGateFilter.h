#pragma once
#ifndef NOISE_GATE_FILTER_H
#define NOISE_GATE_FILTER_H

#include "EnvelopeFollowerFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class NoiseGateFilter : private EnvelopeFollowerFilter<T, SampleRate>
{
public:
	NoiseGateFilter(void)
	{
		SetThreshold(0);

		EnvelopeFollowerFilter<T, SampleRate>::SetAttackTime(0.02);
		EnvelopeFollowerFilter<T, SampleRate>::SetReleaseTime(0.06);
		EnvelopeFollowerFilter<T, SampleRate>::SetUseAbsoluteValue(false);
	}

	//[0dB, 80dB]
	void SetThreshold(float Value)
	{
		ASSERT(0 <= Value && Value <= 80, "Invalid Value %f", Value);

		m_Threshold = Value;
	}

	float GetThreshold(void) const
	{
		return m_Threshold;
	}

	T Process(T Value) override
	{
		T envelope = EnvelopeFollowerFilter<T, SampleRate>::Process(Value);

		if (envelope < m_Threshold)
		{
			T gainFactor = 1 - (envelope / m_Threshold);
			// double gainFactor = Math::Clamp01(1.0 - ((envelope - m_Threshold) / (1 - m_Threshold)));

			Value *= gainFactor;
		}

		return Value;
	}

private:
	float m_Threshold;
};
#endif