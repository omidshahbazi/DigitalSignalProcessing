#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "../Debug.h"
#include "../Math.h"
#include "../Filters/LowPassFilter.h"
#include "../Filters/HighPassFilter.h"

template <typename T, uint32 SampleRate>
class Overdrive : public IDSP<T, SampleRate>
{
public:
	Overdrive(void)
		: m_Drive(0),
		  m_Tone(0),
		  m_WetRate(0)
	{
		SetDrive(0.5);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
		SetWetRate(1);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Drive = Value;

		m_PreGain = (LinearGain)Math::Lerp(10, 100, m_Drive);
		m_InvertedPreGain = (LinearGain)(1 / Math::SquareRoot((float)m_PreGain));

		m_PreFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(100.0, 720.0, m_Drive));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Tone = Value;

		m_PostFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(1 KHz, 8 KHz, m_Tone));
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	//[-12dB, 12dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_Gain = Value;

		m_LinearGain = m_Gain;
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	//[0, 1]
	void SetWetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_WetRate = Value;
	}
	float GetWetRate(void) const
	{
		return m_WetRate;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		CLONE_BUFFER(dryBuffer);

		CREATE_STANDARD_UP_SAMPLE_BUFFER(upBuffer);
		{
			Math::UpSample(Buffer, Count, upBuffer, upBufferFactor);

			m_PreFilter.Process(upBuffer, upBufferLength);

			for (uint8 i = 0; i < upBufferLength; ++i)
				upBuffer[i] = Math::SoftClip(upBuffer[i] * m_PreGain);

			m_PostFilter.Process(upBuffer, upBufferLength);

			for (uint8 i = 0; i < upBufferLength; ++i)
				upBuffer[i] *= m_InvertedPreGain;

			Math::DownSample(upBuffer, upBufferLength, Buffer, upBufferFactor);
		}

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::LinearCrossFadeMix(dryBuffer[i], Buffer[i], m_WetRate);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::SoftClip(Buffer[i]);
	}

private:
	HighPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_PreFilter;
	LowPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_PostFilter;

	float m_Drive;
	LinearGain m_PreGain;
	LinearGain m_InvertedPreGain;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_WetRate;
};

#endif