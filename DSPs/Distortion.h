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
		static typename WaveShaperFilter<T>::TablePoints points[] = {{-1, -0.7}, {-0.1, -0.7}, {0, 0}, {0.4, 1}, {1, 1}};
		m_WaveShaperFilter.SetTable(points, 5);

		m_BandPassFilter.SetFrequencies(100, 1 * KHz);
		m_BandPassFilter.SetResonance(0.3);

		SetLevel(0);
		SetGain(0);
	}

	//[0, 1]
	void SetLevel(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Level = Value;

		m_PreGain = Math::dbToMultiplier(Math::Lerp(-5.0, 20, m_Level));
	}
	float GetLevel(void) const
	{
		return m_Level;
	}

	//[-20dB, 10dB]
	void SetGain(float Value)
	{
		ASSERT(-20 <= Value && Value <= 10, "Invalid Value");

		m_Gain = Value;

		m_PostGain = Math::dbToMultiplier(Value);
	}
	float GetGain(void) const
	{
		return m_Gain;
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T value = Buffer[i];

			value = m_BandPassFilter.Process(value);

			value = m_WaveShaperFilter.Process(value * m_PreGain);

			value *= m_PostGain;

			value = Math::Clamp(value, -1, 1);

			Buffer[i] = value;
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