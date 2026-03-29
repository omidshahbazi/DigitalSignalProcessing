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
		: m_Threshold(0),
		  m_LinearThreshold(0)
	{
		SetThreshold(-65);
		SetAttackTime(20 ms);
		SetReleaseTime(70 ms);
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
	void SetThreshold(dBGain Value)
	{
		ASSERT(-90 <= Value && Value <= 0, "Invalid Value %f", Value);

		m_Threshold = Value;
		m_LinearThreshold = m_Threshold;
	}

	dBGain GetThreshold(void) const
	{
		return m_Threshold;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			LinearGain envelope = EnvelopeFollowerFilter<T, SampleRate>::Process(Buffer[i]);

			LinearGain targetGain = (envelope > m_LinearThreshold) ? 1.0 : 0.0;

			Buffer[i] *= targetGain;
		}
	}

private:
	dBGain m_Threshold;
	LinearGain m_LinearThreshold;
};
#endif