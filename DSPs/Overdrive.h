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
		m_Filter.SetCutoffFrequency(3.5 KHz);

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

	//[-10dB, 6dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-10 <= Value && Value <= 6, "Invalid Value %f", Value);

		m_Gain = Value;

		m_LinearGain = m_Gain;
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_Drive;

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::TanH(Buffer[i]);

		m_Filter.Process(Buffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::SoftClip(Buffer[i]);
	}

private:
	LowPassFilter<T, SampleRate> m_Filter;

	float m_Drive;
	dBGain m_Gain;
	LinearGain m_LinearGain;
};

#endif