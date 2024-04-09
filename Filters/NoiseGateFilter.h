#pragma once
#ifndef NOISE_GATE_FILTER_H
#define NOISE_GATE_FILTER_H

#include "EnvelopeFollowerFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T>
class NoiseGateFilter : private EnvelopeFollowerFilter<T>
{
public:
	NoiseGateFilter(uint32 SampleRate)
		: EnvelopeFollowerFilter<T>(SampleRate)
	{
		SetThreshold(0);

		EnvelopeFollowerFilter<T>::SetAttackTime(0.02);
		EnvelopeFollowerFilter<T>::SetReleaseTime(0.06);
	}

	//[0, 1]
	void SetThreshold(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Threshold = Value;
	}

	float GetThreshold(void) const
	{
		return m_Threshold;
	}

	T Process(T Value) override
	{
		double envelope = EnvelopeFollowerFilter<T>::Process(Value);

		if (envelope < m_Threshold)
		{
			double gainFactor = 1 - (envelope / m_Threshold);
			// double gainFactor = Math::Clamp01(1.0 - ((envelope - m_Threshold) / (1 - m_Threshold)));

			Value *= gainFactor;
		}

		return Value;
	}

private:
	float m_Threshold;
};
#endif