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
		SetThreshold(-65);
		SetAttackTime(20 ms);
		SetReleaseTime(70 ms);
		EnvelopeFollowerFilter<T, SampleRate>::SetUseAbsoluteValue(true);
	}

	//[100ns, 100ms]
	void SetAttackTime(float Value)
	{
		ASSERT(100 ns <= Value && Value <= 100 ms, "Invalid Value %f", Value);

		EnvelopeFollowerFilter<T, SampleRate>::SetAttackTime(Value);
	}
	float GetAttackTime(void) const
	{
		return EnvelopeFollowerFilter<T, SampleRate>::GetAttackTime();
	}

	//[10ms, 2s]
	void SetReleaseTime(float Value)
	{
		ASSERT(10 ms <= Value && Value <= 2, "Invalid Value %f", Value);

		EnvelopeFollowerFilter<T, SampleRate>::SetReleaseTime(Value);
	}
	float GetReleaseTime(void) const
	{
		return EnvelopeFollowerFilter<T, SampleRate>::GetReleaseTime();
	}

	//[-90dB, 0dB]
	void SetThreshold(float Value)
	{
		ASSERT(-90 <= Value && Value <= 0, "Invalid Value %f", Value);

		m_Threshold = Value;
	}

	float GetThreshold(void) const
	{
		return m_Threshold;
	}

	T Process(T Value) override
	{
		T envelope = EnvelopeFollowerFilter<T, SampleRate>::Process(Value);

		T targetGain = (envelope > m_Threshold) ? 1.0 : 0.0;

		return Value * targetGain;
	}

private:
	float m_Threshold;
};
#endif