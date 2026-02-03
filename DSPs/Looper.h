#pragma once
#ifndef LOOPER_H
#define LOOPER_H

#include "IDSP.h"
#include "../Filters/DelayFilter.h"

//How it should work
//https://www.youtube.com/watch?v=b-tNt3fUVAY
template <typename T, uint32 SampleRate, uint16 MaxTime>
class Looper : public IDSP<T, SampleRate>
{
public:
	Looper(void)
		: m_IsReplaying(true),
		  m_FirstRecordIsDone(false),
		  m_Volume(0)
	{
		SetVolume(0.5);
	}

	//(0, MaxTime]
	void SetReplayMode(float RecordLineTime)
	{
		ASSERT(0 < RecordLineTime && RecordLineTime <= MaxTime, "Invalid RecordLineTime");

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
		m_Delay.SetTime(MaxTime);

		m_IsReplaying = true;
		m_FirstRecordIsDone = false;
	}

	float GetMaxTime(void) const
	{
		return m_Delay.GetTime();
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

	void ProcessBuffer(T *Buffer, uint8 Count) override
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

					output = m_Delay.GetSample();
					m_Delay.MoveForward();
				}
			}
			else
				output = m_Delay.Process(input, m_FirstRecordIsDone);

			Buffer[i] = output;
		}
	}

private:
	DelayFilter<T, SampleRate, MaxTime> m_Delay;
	bool m_IsReplaying;
	bool m_FirstRecordIsDone;
	float m_Volume;
};

#endif
