#pragma once
#ifndef TEMPO_METER_H
#define TEMPO_METER_H

#include "IHAL.h"

template <typename T, uint8 ResetTime>
class TempoMeter
{
public:
	TempoMeter(IHAL *HAL)
		: m_HAL(HAL),
		  m_TotalBeatsTime(0),
		  m_LastBeatTime(0),
		  m_Average(0),
		  m_Min(std::numeric_limits<T>::infinity()),
		  m_Max(0),
		  m_BeatCount(0),
		  m_TempoMeterResetTime(0)
	{
	}

	void Update(void)
	{
		if (m_TempoMeterResetTime <= m_HAL->GetTimeSinceStartup())
			Reset();
	}

	void Start()
	{
		m_IsStarted = true;
		m_LastBeatTime = m_HAL->GetTimeSinceStartup();
	}

	void Reset(void)
	{
		m_IsStarted = false;
		m_TotalBeatsTime = 0;
		m_LastBeatTime = 0;
		m_Min = std::numeric_limits<T>::infinity();
		m_Max = 0;
		m_BeatCount = 0;
		
		ResetTimer();
	}

	void Beat(void)
	{
		const float Time = m_HAL->GetTimeSinceStartup();

		++m_BeatCount;
		
		m_TotalBeatsTime += (Time - m_LastBeatTime);

		m_LastBeatTime = Time;

		m_Average = m_BeatCount / (m_TotalBeatsTime / 60);

		if (m_Average < m_Min)
			m_Min = m_Average;
		if (m_Max < m_Average)
			m_Max = m_Average;
			
		ResetTimer();
	}

	bool GetIsStarted(void) const
	{
		return m_IsStarted;
	}

	T GetAverage(void) const
	{
		return m_Average;
	}

	T GetMin(void) const
	{
		return m_Min;
	}

	T GetMax(void) const
	{
		return m_Max;
	}

	uint8 GetBeatCount(void) const
	{
		return m_BeatCount;
	}

private:
	void ResetTimer(void)
	{
		m_TempoMeterResetTime = m_HAL->GetTimeSinceStartup() + ResetTime;
	}

private:
	IHAL *m_HAL;
	bool m_IsStarted;
	T m_TotalBeatsTime;
	T m_LastBeatTime;
	T m_Average;
	T m_Min;
	T m_Max;
	uint8 m_BeatCount;
	float m_TempoMeterResetTime;
};
#endif