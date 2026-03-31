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

		m_BandPassFilter.SetParametersRange(100, 1 KHz, 0.3);

		SetLevel(0);
		SetGain(0);
	}

	//[0, 1]
	void SetLevel(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Level = Value;

		m_PreGain = Math::Lerp(LinearGain(dBGain(-5.0)), LinearGain(dBGain(20)), m_Level);
	}
	float GetLevel(void) const
	{
		return m_Level;
	}

	//[-20dB, 10dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 10, "Invalid Value %f", Value);

		m_Gain = Value;

		m_PostGain = Value;
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		m_BandPassFilter.Process(Buffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_PreGain;

		m_WaveShaperFilter.Process(Buffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_PostGain;

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::Clamp(Buffer[i], -1, 1);
	}

private:
	WaveShaperFilter<T> m_WaveShaperFilter;
	BandPassFilter<T, SampleRate> m_BandPassFilter;

	float m_Level;
	dBGain m_Gain;

	LinearGain m_PreGain;
	LinearGain m_PostGain;
};

#endif