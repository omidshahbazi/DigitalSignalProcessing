#pragma once
#ifndef SINGLE_TONE_CONTROL_FILTER_H
#define SINGLE_TONE_CONTROL_FILTER_H

#include "../Math.h"
#include "LowShelfFilter.h"
#include "HighShelfFilter.h"

template <typename T, uint32 SampleRate>
class SingleToneControlFilter : public Filter<T, SampleRate>
{
public:
	SingleToneControlFilter(void)
		: m_Tone(0)
	{
		SetTone(0.5);
		SetBorderFrequency((Frequency)(1 KHz));
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBorderFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_LowShelfFilter.SetCutoffFrequency(Value);
		m_HighShelfFilter.SetCutoffFrequency(Value);
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Tone = Value;

		const dBGain Gain(6);

		m_LowShelfFilter.SetGain((dBGain)Math::Map(1 - Value, 0.0F, 1, -Gain, Gain));
		m_HighShelfFilter.SetGain((dBGain)Math::Map(Value, 0.0F, 1, -Gain, Gain));
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		m_LowShelfFilter.Process(Buffer, Count);
		m_HighShelfFilter.Process(Buffer, Count);
	}

private:
	float m_Tone;

	LowShelfFilter<T, SampleRate, 1> m_LowShelfFilter;
	HighShelfFilter<T, SampleRate, 1> m_HighShelfFilter;
};

#endif