#pragma once
#ifndef REVERB_H
#define REVERB_H

#include "IDSP.h"
#include "../Filters/DelayFilter.h"

template <typename T>
class Reverb : public IDSP<T>
{
public:
	Reverb(uint32 SampleRate, float MaxDelayTime)
		: m_Delay(SampleRate, MaxDelayTime)
	{
		SetDelayTime(0.5);
		SetFeedback(0.5);
	}

	//[0, MaxDelayTime]
	void SetDelayTime(float Value)
	{
		m_Delay.SetTime(Value);
	}
	float GetDelayTime(void) const
	{
		return m_Delay.GetTime();
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

	//[0, 1]
	void SetWetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Delay.SetOutputMixRate(Value);
	}
	float GetWetRate(void) const
	{
		return m_Delay.GetOutputMixRate();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = m_Delay.Process(Buffer[i], true);
	}

private:
	DelayFilter<T> m_Delay;
};

#endif
