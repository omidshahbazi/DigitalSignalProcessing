#pragma once
#ifndef LOOPER_H
#define LOOPER_H

#include "IDSP.h"
#include "../Filters/DelayFilter.h"

template <typename T>
class Looper : public IDSP<T>
{
public:
	Looper(uint32 SampleRate, float MaxDelayTime)
		: m_Delay(SampleRate, MaxDelayTime),
		  m_IsReplaying(true),
		  m_FirstRecordIsDone(false),
		  m_Volume(0)
	{
		SetVolume(1);
	}

	//(0, ...]
	void SetReplayMode(float Value)
	{
		ASSERT(0 < Value, "Invalid Value");

		if (!m_FirstRecordIsDone)
		{
			m_FirstRecordIsDone = true;

			m_Delay.SetTime(Value);
		}

		m_IsReplaying = true;
	}

	void SetRecordMode(void)
	{
		m_IsReplaying = false;
	}

	void Clear(void)
	{
		m_Delay.Clear();

		m_Mode = Modes::Replay;
		m_FirstRecordIsDone = false;
	}

	//[0, 1]
	void SetVolume(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Volume = Value;
	}
	float GetVolume(void) const
	{
		return m_Volume;
	}

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T input = Buffer[i];
			T delayLine = m_Delay.GetSample();

			if (m_IsReplaying)
				m_Delay.MoveForward();
			else
				Buffer[i] = m_Delay.Process(input, m_FirstRecordIsDone);

			Buffer[i] = (input + (delayLine * m_Volume)) * 0.5;
		}
	}

private:
	DelayFilter<T> m_Delay;
	bool m_IsReplaying;
	bool m_FirstRecordIsDone;
	float m_Volume;
};

#endif
