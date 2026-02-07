#pragma once
#ifndef SINGLE_TONE_CONTROL_FILTER_H
#define SINGLE_TONE_CONTROL_FILTER_H

#include "../Math.h"
#include "LowPassFilter.h"
#include "HighPassFilter.h"

template <typename T, uint32 SampleRate>
class SingleToneControlFilter : public Filter<T, SampleRate>
{
public:
	SingleToneControlFilter(void)
		: m_Tone(1)
	{
		m_LowPassFilter.SetCutoffFrequency(408.0895981378369);
		m_HighPassFilter.SetCutoffFrequency(1476.390939459707);
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Tone = Value;
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	T Process(T Value) override
	{
		return Math::Lerp(m_LowPassFilter.Process(Value), m_HighPassFilter.Process(Value), m_Tone);
	}

private:
	float m_Tone;

	LowPassFilter<T, SampleRate> m_LowPassFilter;
	HighPassFilter<T, SampleRate> m_HighPassFilter;
};
#endif