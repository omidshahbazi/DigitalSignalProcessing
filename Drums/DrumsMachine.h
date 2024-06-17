#pragma once
#ifndef DRUMS_MATCHINE_H
#define DRUMS_MATCHINE_H

#include "../IHAL.h"
#include "Snare.h"

template <typename T, uint32 SampleRate>
class DrumsMachine
{
	ASSERT_ON_FLOATING_TYPE(T);
	ASSERT_ON_SAMPLE_RATE(SampleRate);

public:
	DrumsMachine(IHAL *HAL)
		: m_HAL(HAL),
		  m_BeatsPerMinute(0),
		  m_BeatTime(0),
		  m_NextBeatTime(0)
	{
		SetBeatsPerMinute(60);
	}

	//[20, 400]
	void SetBeatsPerMinute(float Value)
	{
		m_BeatsPerMinute = Value;

		m_BeatTime = 60 * 1000 / m_BeatsPerMinute;
	}
	float GetBeatsPerMinute(void) const
	{
		return m_BeatsPerMinute;
	}

	void Update(void)
	{
		uint32 time = m_HAL->GetTimeSinceStartupMs();
		if (time >= m_NextBeatTime)
		{
			m_Snare.Beat();

			m_NextBeatTime = time + m_BeatTime;
		}
	}

	void ProcessBuffer(T *Buffer, uint8 Count)
	{
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = m_Snare.Process();
	}

private:
	IHAL *m_HAL;
	float m_BeatsPerMinute;
	uint32 m_BeatTime;
	uint32 m_NextBeatTime;

	Snare<T, SampleRate> m_Snare;
};

#endif