#pragma once
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "IDSP.h"
#include "../Debug.h"
#include "../Math.h"
#include "../Filters/EnvelopeFollowerFilter.h"

template <typename T, uint32 SampleRate>
class Compressor : public IDSP<T, SampleRate>
{
public:
	Compressor(void)
		: m_Ratio(2.0f),
		  m_Threshold(0),
		  m_MakeupGain(1.0f)
	{
		SetAttackTime(10 ms);
		SetReleaseTime(100 ms);
		SetRatio(2.0f);
		SetThreshold(-12);
	}

	//[100ns, 500ms]
	void SetAttackTime(float Value)
	{
		ASSERT(100 ns <= Value && Value <= 500 ms, "Invalid Value %f", Value);

		m_EnvelopeFollowerFilter.SetAttackTime(Value);
	}
	float GetAttackTime(void) const
	{
		return m_EnvelopeFollowerFilter.GetAttackTime();
	}

	//[10ms, 2s]
	void SetReleaseTime(float Value)
	{
		ASSERT(10 ms <= Value && Value <= 2, "Invalid Value %f", Value);

		m_EnvelopeFollowerFilter.SetReleaseTime(Value);
	}
	float GetReleaseTime(void) const
	{
		return m_EnvelopeFollowerFilter.GetReleaseTime();
	}

	//[1, 40]
	void SetRatio(float Value)
	{
		ASSERT(1 <= Value && Value <= 40, "Invalid Value %f", Value);

		m_Ratio = Value;
	}
	float GetRatio(void) const
	{
		return m_Ratio;
	}

	//[-80dB, NORMAL_GAIN]
	void SetThreshold(dBGain Value)
	{
		ASSERT(-80 <= Value && Value <= NORMAL_GAIN, "Invalid Value %f", Value);

		m_Threshold = Value;
	}
	dBGain GetThreshold(void) const
	{
		return m_Threshold;
	}

	//[NORMAL_GAIN, 24dB]
	void SetMakeupGain(dBGain Value)
	{
		ASSERT(NORMAL_GAIN <= Value && Value <= 24, "Invalid Value %f", Value);

		m_MakeupGain = Value;
		m_MakeupGainLinear = m_MakeupGain;
	}
	dBGain GetMakeupGain(void) const
	{
		return m_MakeupGain;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			dBGain envelop = (LinearGain)m_EnvelopeFollowerFilter.Process(Math::Absolute(Buffer[i]));

			dBGain gainReduction = 0;
			if (envelop > m_Threshold)
				gainReduction = (1.0f - (1.0f / m_Ratio)) * (m_Threshold - envelop);

			float multiplier = LinearGain(gainReduction) * m_MakeupGainLinear;

			Buffer[i] = Math::SoftClip(Buffer[i] * multiplier);
		}
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	float m_Ratio;
	dBGain m_Threshold;
	dBGain m_MakeupGain;
	LinearGain m_MakeupGainLinear;
};

#endif