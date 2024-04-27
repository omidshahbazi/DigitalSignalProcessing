#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/BandPassFilter.h"

template <typename T>
class Overdrive : public IDSP<T>
{
public:
	Overdrive(uint32 SampleRate)
		: m_BandPassFilter(SampleRate),
		  m_Drive(0),
		  m_Gain(0)
	{
		m_BandPassFilter.SetFrequencies(100, 5 * KHz);

		SetGain(1);
		SetDrive(1);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Drive = Value;
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[0, 1]
	void SetGain(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Gain = Value;
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
			Buffer[i] = Math::SoftClip(Buffer[i] * (m_Drive + 1)) * (m_Gain + 1);
		}
	}

private:
	BandPassFilter<T> m_BandPassFilter;
	float m_Drive;
	float m_Gain;
};

#endif