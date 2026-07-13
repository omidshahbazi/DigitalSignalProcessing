#pragma once
#ifndef BIG_MUFF_H
#define BIG_MUFF_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/UpSamplerFilter.h"
#include "../Filters/HighPassFilter.h"
#include "../Filters/LowPassFilter.h"
#include "../Filters/ToneStackFilter.h"

template <typename T, uint32 SampleRate>
class BigMuff : public IDSP<T, SampleRate>
{
public:
	BigMuff(void)
		: m_Drive(0),
		m_Tone(0),
		m_WetRate(0)
	{
		m_PreFilter.SetCutoffFrequency(Frequency(200));
		m_DCBlockerFilter.SetCutoffFrequency(Frequency(5));
		m_ToneStackFilter.SetLowPassFrequency(Frequency(200));
		m_ToneStackFilter.SetHighPassFrequency(Frequency(1.4 KHz));

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

		m_PreGain = (LinearGain)Math::SquareRoot(Math::Lerp(10, 800, m_Drive));
		m_InvertedPreGain = (LinearGain)(1 / Math::SquareRoot((float)m_PreGain));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_ToneStackFilter.SetTone(Value);
	}
	float GetTone(void) const
	{
		return m_ToneStackFilter.GetTone();
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

	void Process(T* Buffer, uint8 Count) override
	{
		CLONE_BUFFER(dryBuffer);

		m_PreFilter.Process(Buffer, Count);

		T* upBuffer = m_UpSampler.Process(Buffer);
		{
			for (uint8 i = 0; i < m_UpSampler.GetCount(); ++i)
			{
				upBuffer[i] = Math::SoftClip(upBuffer[i], m_PreGain, 0.1);
				upBuffer[i] = Math::SoftClip(upBuffer[i], m_PreGain, 0.05) * m_InvertedPreGain;
			}

			m_UpSampler.DownSample(Buffer);
		}

		m_DCBlockerFilter.Process(Buffer, Count);
		m_ToneStackFilter.Process(Buffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::LinearCrossFadeMix(dryBuffer[i], Buffer[i] * m_LinearGain, m_WetRate);
	}

private:
	HighPassFilter<T, SampleRate> m_PreFilter;
	UpSamplerFilter<T, SampleRate, FrameLength, STANDARD_UP_SAMPLE_FACTOR, true> m_UpSampler;
	HighPassFilter<T, SampleRate> m_DCBlockerFilter;
	ToneStackFilter<T, SampleRate> m_ToneStackFilter;

	float m_Drive;
	LinearGain m_PreGain;
	LinearGain m_InvertedPreGain;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_WetRate;
};

#endif