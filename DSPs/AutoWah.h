#pragma once
#ifndef AUTO_WAH_H
#define AUTO_WAH_H

#include "Wah.h"
#include "../Filters/EnvelopeFollowerFilter.h"

template <typename T, uint32 SampleRate>
class AutoWah : private Wah<T, SampleRate>
{
#define ENVELOP_AVERAGE_DURATION 0.1

public:
	AutoWah(void)
		: m_MaxEnvelopSampleCount(ENVELOP_AVERAGE_DURATION * SampleRate),
		  m_EnvelopeSum(0),
		  m_EnvelopeSampleCount(0)
	{
		m_EnvelopeFollowerFilter.SetAttackTime(0.015841);
		m_EnvelopeFollowerFilter.SetReleaseTime(0.045401);
		m_EnvelopeFollowerFilter.SetUseAbsoluteValue(false);
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			m_EnvelopeSum += Math::Absolute(m_EnvelopeFollowerFilter.Process(Buffer[i]));
			if (++m_EnvelopeSampleCount == m_MaxEnvelopSampleCount)
			{
				float ratio = m_EnvelopeSum / m_EnvelopeSampleCount;
				ratio *= 125;
				ratio = Math::Clamp01(ratio);

				Wah<T, SampleRate>::SetRatio(ratio);

				m_EnvelopeSum = 0;
				m_EnvelopeSampleCount = 0;
			}

			Buffer[i] = Wah<T, SampleRate>::Process(Buffer[i]);
		}
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	uint32 m_MaxEnvelopSampleCount;
	T m_EnvelopeSum;
	uint32 m_EnvelopeSampleCount;
};
#endif
