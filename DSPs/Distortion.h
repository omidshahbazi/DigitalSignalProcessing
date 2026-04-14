#pragma once
#ifndef DISTORTION_H
#define DISTORTION_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/LowPassFilter.h"
#include "../Filters/HighPassFilter.h"

template <typename T, uint32 SampleRate>
class Distortion : public IDSP<T, SampleRate>
{
public:
	Distortion(void)
		: m_Level(0),
		  m_InvertedLevel(0),
		  m_AsymmetryLevel(0),
		  m_WetRate(0)
	{
		SetBassFilter(Frequency(800));
		m_PostFilter.SetCutoffFrequency(Frequency(2.5 KHz));
		m_AAFilter.SetCutoffFrequency(Frequency(20 KHz));

		SetLevel(50);
		SetGain(NORMAL_GAIN);
		SetAsymmetryLevel(0);
		SetWetRate(0.5);
	}

	// [150, 800]
	void SetBassFilter(Frequency Value)
	{
		ASSERT(150 <= Value && Value <= 800, "Invalid Value %f", Value);

		m_PreFilter.SetCutoffFrequency(Value);
	}
	Frequency GetBassFilter(void) const
	{
		return m_PreFilter.GetCutoffFrequency();
	}

	//[1, 240]
	void SetLevel(float Value)
	{
		ASSERT(1 <= Value && Value <= 240, "Invalid Value %f", Value);

		m_Level = Value;
		m_InvertedLevel = 1 / (m_Level * 0.5);
	}
	float GetLevel(void) const
	{
		return m_Level;
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

	//[-20dB, 10dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 10, "Invalid Value %f", Value);

		m_Gain = Value;

		m_LinearGain = Value;
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
		m_PreFilter.Process(Buffer, Count);

		CREATE_STANDARD_UP_SAMPLE_BUFFER(upBuffer);
		{
			Math::UpSample(Buffer, Count, upBuffer, upBufferFactor);

			for (uint8 i = 0; i < upBufferLength; ++i)
			{
				T wet = Math::AsymmetricGain(upBuffer[i], m_AsymmetryLevel);

				wet = Math::HardClip(wet * m_Level, 0.3);// * m_InvertedLevel;

				upBuffer[i] = Math::LinearCrossFadeMix(upBuffer[i], wet, m_WetRate);
			}

			// m_AAFilter.Process(upBuffer, upBufferLength);

			Math::DownSample(upBuffer, upBufferLength, Buffer, upBufferFactor);
		}

		m_PostFilter.Process(Buffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::ClampSignal(Buffer[i]);
	}

private:
	HighPassFilter<T, SampleRate> m_PreFilter;
	LowPassFilter<T, SampleRate> m_PostFilter;
	LowPassFilter<T, SampleRate * STANDARD_UP_SAMPLE_FACTOR> m_AAFilter;

	float m_Level;
	float m_InvertedLevel;
	float m_AsymmetryLevel;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_WetRate;
};

#endif