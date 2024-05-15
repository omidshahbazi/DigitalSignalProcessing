#pragma once
#ifndef DISTORTION_H
#define DISTORTION_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/WaveShaperFilter.h"
#include "../Filters/BandPassFilter.h"

template <typename T>
class Distortion : public IDSP<T>
{
public:
	Distortion(uint32 SampleRate)
		: m_BandPassFilter(SampleRate),
		  m_Rate(0),
		  m_Gain(0),
		  m_PreGain(0),
		  m_PostGain(0)
	{
		static typename WaveShaperFilter<T>::TablePoints points[] = {{-1, -1}, {-0.5, -1}, {0, 0}, {0.5, 1}, {1, 1}};
		m_WaveShaperFilter.SetTable(points, 5);

		m_BandPassFilter.SetFrequencies(150, 2 * KHz);

		SetGain(1);
		SetRate(1);
	}

	//[0, 1]
	void SetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Rate = Value;

		m_PreGain = Math::Lerp(1.0, 4, m_Rate);
	}
	float GetRate(void) const
	{
		return m_Rate;
	}

	//[0, 1]
	void SetGain(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Gain = Value;

		m_PostGain = Math::Lerp(0.5, 1, m_Gain);
	}
	float GetGain(void) const
	{
		return m_Gain;
	}

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			Buffer[i] = m_BandPassFilter.Process(Buffer[i]) * 40;
			Buffer[i] += (1 - m_PostGain);
			Buffer[i] = m_WaveShaperFilter.Process(Buffer[i] * m_PreGain) * m_PostGain;
		}
	}

private:
	WaveShaperFilter<T> m_WaveShaperFilter;
	BandPassFilter<T> m_BandPassFilter;

	float m_Rate;
	float m_Gain;

	float m_PreGain;
	float m_PostGain;
};

#endif