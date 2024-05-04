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
	void SetReplayMode(float RecordLineTime)
	{
		ASSERT(0 < RecordLineTime, "Invalid RecordLineTime");

		if (!m_FirstRecordIsDone)
		{
			m_FirstRecordIsDone = true;

			m_Delay.SetTime(RecordLineTime);
		}

		m_IsReplaying = true;
	}

	void SetRecordMode(void)
	{
		m_IsReplaying = false;
	}

	void Clear(void)
	{
		m_Delay.Reset();

		m_IsReplaying = true;
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
			T output = input;

			if (m_IsReplaying)
			{
				if (m_FirstRecordIsDone)
				{
					output = Math::Lerp(input, m_Delay.GetSample(), m_Volume);

					m_Delay.MoveForward();
				}
			}
			else
				output = m_Delay.Process(input, m_FirstRecordIsDone);

			Buffer[i] = output;
		}
	}

private:
	DelayFilter<T> m_Delay;
	bool m_IsReplaying;
	bool m_FirstRecordIsDone;
	float m_Volume;
};

#endif
