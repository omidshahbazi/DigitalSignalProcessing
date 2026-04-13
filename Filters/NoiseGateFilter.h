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
		: m_IsGateOpen(false),
		  m_CurrentGain(0)
	{
		SetThreshold(dBGain(-65));
		SetHysteresis(dBGain(6));
		SetSmoothing(0.01);
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
		m_LinearOpenThreshold = m_Threshold;

		UpdateCloseThreshold();
	}
	dBGain GetThreshold(void) const
	{
		return m_Threshold;
	}

	//[0dB, 12dB]
	void SetHysteresis(dBGain Value)
	{
		ASSERT(0 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_Hysteresis = Value;

		UpdateCloseThreshold();
	}
	dBGain GetHysteresis(void) const
	{
		return m_Hysteresis;
	}

	//(0, 1]
	void SetSmoothing(float Value)
	{
		ASSERT(0 < Value && Value < 1, "Invalid Value %f", Value);

		m_Smoothing = Value;
	}
	float GetSmoothing(void) const
	{
		return m_Smoothing;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			LinearGain envelope = (LinearGain)EnvelopeFollowerFilter<T, SampleRate>::Process(Buffer[i]);

			if (envelope > m_LinearOpenThreshold)
				m_IsGateOpen = true;
			else if (envelope < m_LinearCloseThreshold)
				m_IsGateOpen = false;

			LinearGain targetGain = (LinearGain)(m_IsGateOpen ? 1 : 0);
			m_CurrentGain = LinearGain(m_CurrentGain + ((targetGain - m_CurrentGain) * m_Smoothing));

			Buffer[i] *= m_CurrentGain;
		}
	}

private:
	void UpdateCloseThreshold(void)
	{
		m_LinearCloseThreshold = (LinearGain)dBGain(m_Threshold - m_Hysteresis);
	}

private:
	dBGain m_Threshold;
	dBGain m_Hysteresis;
	float m_Smoothing;

	LinearGain m_LinearOpenThreshold;
	LinearGain m_LinearCloseThreshold;

	bool m_IsGateOpen;
	LinearGain m_CurrentGain;
};
#endif