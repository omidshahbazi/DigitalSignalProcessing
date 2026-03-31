#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "../Filters/LowPassFilter.h"
#include "../Filters/HighPassFilter.h"
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
		m_PreFilter.SetCutoffFrequency(720);
		m_PostFilter.SetCutoffFrequency(4.5 KHz);

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
		CREATE_STANDARD_UP_SAMPLE_BUFFER(upBuffer);

		Math::UpSample(Buffer, Count, upBuffer, upBufferFactor);

		m_PreFilter.Process(upBuffer, upBufferLength);

		for (uint8 i = 0; i < upBufferLength; ++i)
			upBuffer[i] = Math::CrunchClip(upBuffer[i] * m_Drive);

		m_PostFilter.Process(upBuffer, upBufferLength);

		Math::DownSample(upBuffer, upBufferLength, Buffer, upBufferFactor);

		for (uint8 i = 0; i < Count; ++i)
		{
			Buffer[i] *= m_LinearGain;
			Buffer[i] = Math::SoftClip(Buffer[i]);
		}
	}

private:
	HighPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_PreFilter;
	LowPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_PostFilter;

	float m_Drive;
	dBGain m_Gain;
	LinearGain m_LinearGain;
};

#endif