#pragma once
#ifndef FUZZ_H
#define FUZZ_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/UpSamplerFilter.h"
#include "../Filters/HighPassFilter.h"
#include "../Filters/LowPassFilter.h"

template <typename T, uint32 SampleRate>
class Fuzz : public IDSP<T, SampleRate>
{
public:
	Fuzz(void)
		: m_Drive(0),
		  m_AsymmetryLevel(0),
		  m_Tone(0),
		  m_WetRate(0)
	{
		m_DCBlockerFilter.SetCutoffFrequency(Frequency(5));

		SetDrive(0.5);
		SetAsymmetryLevel(0);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
		SetWetRate(1);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Drive = Value;

		m_PreGain = (LinearGain)Math::Lerp(10, 500, m_Drive);
		m_InvertedPreGain = (LinearGain)(1 / Math::SquareRoot((float)m_PreGain));
		m_PreFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(200.0, 400.0, m_Drive));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[-1, 1]
	void SetAsymmetryLevel(float Value)
	{
		ASSERT(-1 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_AsymmetryLevel = Value;
	}
	float GetAsymmetryLevel(void) const
	{
		return m_AsymmetryLevel;
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Tone = Value;

		m_ToneFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(1 KHz, 8 KHz, m_Tone));
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

		m_PreFilter.Process(Buffer, Count);

		T *upBuffer = m_UpSampler.Process(Buffer);
		{
			for (uint8 i = 0; i < m_UpSampler.GetCount(); ++i)
				upBuffer[i] = Math::HardClip(upBuffer[i], m_PreGain, 0.7, m_AsymmetryLevel) * m_InvertedPreGain;

			m_UpSampler.DownSample(Buffer);
		}

		m_DCBlockerFilter.Process(Buffer, Count);
		m_ToneFilter.Process(Buffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::LinearCrossFadeMix(dryBuffer[i], Buffer[i], m_WetRate);
	}

private:
	HighPassFilter<T, SampleRate> m_PreFilter;
	UpSamplerFilter<T, SampleRate, FrameLength, STANDARD_UP_SAMPLE_FACTOR, true> m_UpSampler;
	HighPassFilter<T, SampleRate> m_DCBlockerFilter;
	LowPassFilter<T, SampleRate> m_ToneFilter;

	float m_Drive;
	LinearGain m_PreGain;
	LinearGain m_InvertedPreGain;
	float m_AsymmetryLevel;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_WetRate;
};

#endif