#pragma once
#ifndef NOISE_GATE_FILTER_H
#define NOISE_GATE_FILTER_H

#include "EnvelopeFollowerFilter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"

template <typename T, uint32_t SampleRate>
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
		SetReleaseTime(200 ms);
	}

	//[100ns, 100ms]
	void SetAttackTime(float Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, 100 ns, 100 ms);

		EnvelopeFollowerFilter<T, SampleRate>::SetAttackTime(Value);
	}
	float GetAttackTime(void) const
	{
		return EnvelopeFollowerFilter<T, SampleRate>::GetAttackTime();
	}

	//[10ms, 2s]
	void SetReleaseTime(float Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, 10 ms, 2);

		EnvelopeFollowerFilter<T, SampleRate>::SetReleaseTime(Value);
	}
	float GetReleaseTime(void) const
	{
		return EnvelopeFollowerFilter<T, SampleRate>::GetReleaseTime();
	}

	void SetThreshold(cc_dBGain<MIN_GAIN_VALUE, NORMAL_GAIN_VALUE> Value)
	{
		m_Threshold = Value;
		m_LinearOpenThreshold = m_Threshold;

		UpdateCloseThreshold();
	}
	dBGain GetThreshold(void) const
	{
		return m_Threshold;
	}

	void SetHysteresis(cc_dBGain<NORMAL_GAIN_VALUE, 12> Value)
	{
		m_Hysteresis = Value;

		UpdateCloseThreshold();
	}
	dBGain GetHysteresis(void) const
	{
		return m_Hysteresis;
	}

	void SetSmoothing(oc_ratio_t Value)
	{
		m_Smoothing = Value;
	}
	float GetSmoothing(void) const
	{
		return m_Smoothing;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
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
		m_LinearCloseThreshold = (LinearGain)(m_Threshold - m_Hysteresis);
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