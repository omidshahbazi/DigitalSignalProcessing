#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "../Filters/LowPassFilter.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class Overdrive : public IDSP<T, SampleRate>
{
public:
	Overdrive(void)
		: m_Drive(0),
		  m_Gain(0),
		  m_LinearGain(0)
	{
		m_Filter.SetCutoffFrequency(3.5 * KHz);
		m_Filter.SetResonance(RESONANCE_BUTTERWORTH);

		SetDrive(1);
		SetGain(0);
	}

	//[1, 50]
	void SetDrive(float Value)
	{
		ASSERT(1 <= Value && Value <= 50, "Invalid Value %f", Value);

		m_Drive = Value;
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[-6dB, 6dB]
	void SetGain(float Value)
	{
		ASSERT(-6 <= Value && Value <= 6, "Invalid Value %f", Value);

		m_Gain = Value;

		m_LinearGain = Math::dBToLinear(m_Gain);
	}
	float GetGain(void) const
	{
		return m_Gain;
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T value = Math::TanH(Buffer[i] * m_Drive);

			value = m_Filter.Process(value);

			Buffer[i] = value * m_LinearGain;
		}
	}

private:
	LowPassFilter<T, SampleRate> m_Filter;

	float m_Drive;
	float m_Gain;
	float m_LinearGain;
};

#endif