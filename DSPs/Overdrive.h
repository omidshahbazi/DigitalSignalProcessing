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
		  m_LinearGain(0),
		  m_AsymmetryLevel(0),
		  m_WetRate(0)
	{
		SetBassFilter(720);
		m_DCOffsetFilter.SetCutoffFrequency(5);
		m_PostFilter.SetCutoffFrequency(4.5 KHz);

		SetDrive(25);
		SetGain(0);
		SetAsymmetryLevel(0);
		SetWetRate(0.5);
	}

	// [MIN_FREQUENCY, 800]
	void SetBassFilter(float Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= 800, "Invalid Value %f", Value);

		m_PreFilter.SetCutoffFrequency(Value);
	}
	float GetBassFilter(void) const
	{
		return m_PreFilter.GetCutoffFrequency();
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

	//[0, 1)
	void SetAsymmetryLevel(float Value)
	{
		ASSERT(0 <= Value && Value < 1, "Invalid Value %f", Value);

		m_AsymmetryLevel = Value;
	}
	float GetAsymmetryLevel(void) const
	{
		return m_AsymmetryLevel;
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
		CREATE_STANDARD_UP_SAMPLE_BUFFER(upBuffer);
		{
			Math::UpSample(Buffer, Count, upBuffer, upBufferFactor);

			m_PreFilter.Process(upBuffer, upBufferLength);

			for (uint8 i = 0; i < upBufferLength; ++i)
			{
				T wet = Math::AsymmetricGain(upBuffer[i], m_AsymmetryLevel);

				wet = Math::CrunchClip(wet * m_Drive, 0.2);

				upBuffer[i] = Math::LinearCrossFadeMix(upBuffer[i] * m_Drive, wet, m_WetRate);
			}

			m_DCOffsetFilter.Process(upBuffer, upBufferLength);
			m_PostFilter.Process(upBuffer, upBufferLength);

			Math::DownSample(upBuffer, upBufferLength, Buffer, upBufferFactor);
		}

		for (uint8 i = 0; i < Count; ++i)
		{
			Buffer[i] *= m_LinearGain;
			Buffer[i] = Math::SoftClip(Buffer[i]);
		}
	}

private:
	HighPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_PreFilter;
	HighPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_DCOffsetFilter;
	LowPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_PostFilter;

	float m_Drive;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_AsymmetryLevel;
	float m_WetRate;
};

#endif