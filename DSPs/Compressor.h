#pragma once
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "IDSP.h"
#include "../Debug.h"
#include "../Math.h"
#include "../Filters/EnvelopeFollowerFilter.h"

// TODO: Algorithm seems incorrect
template <typename T, uint32 SampleRate>
class Compressor : public IDSP<T, SampleRate>
{
public:
	Compressor(void)
		: m_Ratio(0),
		  m_Threshold(0),
		  m_AttackSlope2(0),
		  m_RatioMultipler(0),
		  m_LastGain(0.1)
	{
		m_EnvelopeFollowerFilter.SetUseAbsoluteValue(true);

		SetAttackTime(0.1);
		SetReleaseTime(0.1);

		SetRatio(2);
		SetThreshold(-12);
	}

	//[0.001, 10]
	void SetAttackTime(float Value)
	{
		m_EnvelopeFollowerFilter.SetAttackTime(Value);

		m_AttackSlope2 = Math::Exponential(-((1 / SampleRate) / Value));
	}
	float GetAttackTime(void) const
	{
		return m_EnvelopeFollowerFilter.GetAttackTime();
	}

	//[0.001, 10]
	void SetReleaseTime(float Value)
	{
		m_EnvelopeFollowerFilter.SetReleaseTime(Value);
	}
	float GetReleaseTime(void) const
	{
		return m_EnvelopeFollowerFilter.GetReleaseTime();
	}

	//[1, 40]
	void SetRatio(float Value)
	{
		ASSERT(1 <= Value && Value <= 40, "Invalid Value");

		m_Ratio = Value;

		m_RatioMultipler = (1 - m_AttackSlope2) * ((1 / m_Ratio) - 1);
	}
	float GetRatio(void) const
	{
		return m_Ratio;
	}

	//[-80dB, 0dB]
	void SetThreshold(float Value)
	{
		ASSERT(-80 <= Value && Value <= 0, "Invalid Value");

		m_Threshold = Value;

		Log::WriteInfo("Threshold %f", m_Threshold);
	}
	float GetThreshold(void) const
	{
		return m_Threshold;
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T envelope = m_EnvelopeFollowerFilter.Process(Buffer[i]);

			m_LastGain = ((m_AttackSlope2 * m_LastGain) + (m_RatioMultipler * Math::Max((20 * Math::Log10(envelope)) - m_Threshold, 0)));

			float gain = Math::Power10(0.05 * m_LastGain);

			Buffer[i] *= gain;

			Buffer[i] = Math::Clamp(Buffer[i], -1, 1);

			Buffer[i] = m_LastGain;
		}
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	float m_Ratio;
	float m_Threshold;

	float m_AttackSlope2;
	float m_RatioMultipler;
	float m_LastGain;
};

#endif
