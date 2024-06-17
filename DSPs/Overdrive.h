#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "../Filters/LowPassFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class Overdrive : public IDSP<T, SampleRate>
{
public:
	Overdrive(void)
		: m_Drive(0),
		  m_Gain(0),
		  m_PreGain(0),
		  m_PostGain(0)
	{
		m_Filter.SetCutoffFrequency(3 * KHz);
		m_Filter.SetResonance(2);

		SetDrive(1);
		SetGain(0);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Drive = Value;

		m_PreGain = Math::Lerp(1.0, 4, m_Drive);
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[-6dB, 6dB]
	void SetGain(float Value)
	{
		ASSERT(-6 <= Value && Value <= 6, "Invalid Value");

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

			value = m_Filter.Process(value);

			value = Math::SoftClip(value * m_PreGain);

			value *= m_PostGain;

			value = Math::Clamp(value, -1, 1);

			Buffer[i] = value;
		}
	}

private:
	LowPassFilter<T, SampleRate> m_Filter;

	float m_Drive;
	float m_Gain;

	float m_PreGain;
	float m_PostGain;
};

#endif