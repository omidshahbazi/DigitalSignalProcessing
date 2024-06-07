#pragma once
#ifndef DISTORTION_H
#define DISTORTION_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/WaveShaperFilter.h"
#include "../Filters/BandPassFilter.h"

template <typename T, uint32 SampleRate>
class Distortion : public IDSP<T, SampleRate>
{
public:
	Distortion(void)
		: m_Level(0),
		  m_Gain(0),
		  m_PreGain(0),
		  m_PostGain(0)
	{
		static typename WaveShaperFilter<T>::TablePoints points[] = {{-1, -1}, {-0.5, -1}, {0, 0}, {0.5, 1}, {1, 1}};
		m_WaveShaperFilter.SetTable(points, 5);

		m_BandPassFilter.SetFrequencies(150, 2 * KHz);

		SetLevel(1);
		SetGain(0);
	}

	//[0, 1]
	void SetLevel(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Level = Value;

		m_PreGain = 1 + Math::dbToMultiplier(Math::Lerp(0, 100, m_Level));
	}
	float GetLevel(void) const
	{
		return m_Level;
	}

	//[-10dB, 10dB]
	void SetGain(float Value)
	{
		ASSERT(-10 <= Value && Value <= 10, "Invalid Value");

		m_Gain = Value;

		m_PostGain = 1 + Math::dbToMultiplier(Value);
	}
	float GetGain(void) const
	{
		return m_Gain;
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			Buffer[i] = m_BandPassFilter.Process(Buffer[i]);
			// Buffer[i] += (1 - m_PostGain);
			Buffer[i] = m_WaveShaperFilter.Process(Buffer[i] * m_PreGain) * m_PostGain;
		}
	}

private:
	WaveShaperFilter<T> m_WaveShaperFilter;
	BandPassFilter<T, SampleRate> m_BandPassFilter;

	float m_Level;
	float m_Gain;

	float m_PreGain;
	float m_PostGain;
};

#endif