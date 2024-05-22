#pragma once
#ifndef SUSTAIN_H
#define SUSTAIN_H

#include "IDSP.h"
#include "../Filters/DelayFilter.h"

template <typename T>
class Sustain : public IDSP<T>
{
public:
	Sustain(uint32 SampleRate, float MaxDelayTime)
		: m_Delay(SampleRate, MaxDelayTime),
		  m_Active(false),
		  m_Wet(false)
	{
		SetFeedback(1);
	}

	//[0, 1]
	void SetFeedback(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Delay.SetFeedback(Value);
	}
	float GetFeedback(void) const
	{
		return m_Delay.GetFeedback();
	}

	void SetActive(bool Value)
	{
		m_Active = Value;

		printf("Act %i\n", Value);
	}
	bool GetActive(void) const
	{
		return m_Active;
	}

	void SetWet(bool Value)
	{
		m_Wet = Value;

		printf("Wet %i\n", Value);
	}
	bool GetWet(void) const
	{
		return m_Wet;
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T delayedSample = m_Delay.GetSample(m_DelayOffset);
			m_DelayOffset = (m_DelayOffset + 1) % m_Delay.GetBufferLength();

			if (m_Active)
			{
				if (m_Wet)
					Buffer[i] = delayedSample;
				else
					Buffer[i] += delayedSample;
			}
			else
				m_Delay.Process(Buffer[i]);
		}
	}

private:
	DelayFilter<T> m_Delay;
	bool m_Active;
	bool m_Wet;

	uint32 m_DelayOffset;
};

#endif