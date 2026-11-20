#pragma once
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Filters/EnvelopeFollowerFilter.h"

template <typename T, uint32_t SampleRate>
class Compressor : public IDSP<T, SampleRate>
{
public:
	Compressor(void)
	{
		SetAttackTime(10 ms);
		SetReleaseTime(100 ms);
		SetRatio(1);
		SetThreshold(dBGain(-18));
		SetMakeupGain(NORMAL_GAIN);
	}

	//[100ns, 500ms]
	void SetAttackTime(float Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, 100 ns, 500 ms);

		m_EnvelopeFollowerFilter.SetAttackTime(Value);
	}
	float GetAttackTime(void) const
	{
		return m_EnvelopeFollowerFilter.GetAttackTime();
	}

	//[10ms, 2s]
	void SetReleaseTime(float Value)
	{
		ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, 10 ms, 2);

		m_EnvelopeFollowerFilter.SetReleaseTime(Value);
	}
	float GetReleaseTime(void) const
	{
		return m_EnvelopeFollowerFilter.GetReleaseTime();
	}

	void SetRatio(cc_ratio_t Value)
	{
		m_Ratio = Value;

		m_RatioValue = Math::Lerp(1, 10, m_Ratio);
	}
	float GetRatio(void) const
	{
		return m_Ratio;
	}

	void SetThreshold(cc_dBGain<MIN_GAIN_VALUE, NORMAL_GAIN_VALUE> Value)
	{
		m_Threshold = Value;
	}
	dBGain GetThreshold(void) const
	{
		return m_Threshold;
	}

	void SetMakeupGain(cc_dBGain<NORMAL_GAIN_VALUE, 24> Value)
	{
		m_MakeupGain = Value;
	}
	dBGain GetMakeupGain(void) const
	{
		return m_MakeupGain;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			dBGain envelop = (LinearGain)m_EnvelopeFollowerFilter.Process(Math::Absolute(Buffer[i]));

			dBGain gainReduction;
			if (envelop > m_Threshold)
				gainReduction = dBGain(((1 / m_RatioValue) - 1) * (envelop - m_Threshold));

			LinearGain totalGain = gainReduction + m_MakeupGain;

			Buffer[i] *= totalGain;
		}
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	float m_Ratio;
	float m_RatioValue;
	dBGain m_Threshold;
	dBGain m_MakeupGain;
};

#endif