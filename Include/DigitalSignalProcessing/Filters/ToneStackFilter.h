#pragma once
#ifndef TONE_STACK_FILTER_H
#define TONE_STACK_FILTER_H

#include "../Math.h"
#include "LowPassFilter.h"
#include "HighPassFilter.h"

template <typename T, uint32 SampleRate>
class ToneStackFilter : public Filter<T, SampleRate>
{
public:
	ToneStackFilter(void)
		: m_Tone(0)
	{
		SetTone(0.5);
		SetLowPassFrequency((Frequency)(200));
		SetHighPassFrequency((Frequency)(1 KHz));
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetLowPassFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_LowPassFilter.SetCutoffFrequency(Value);
	}

	Frequency GetLowPassFrequency(void) const
	{
		return m_LowPassFilter.GetCutoffFrequency();
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetHighPassFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_HighPassFilter.SetCutoffFrequency(Value);
	}

	Frequency GetHighPassFrequency(void) const
	{
		return m_HighPassFilter.GetCutoffFrequency();
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

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::LinearCrossFadeMix(m_LowPassFilter.Process(Buffer[i]), m_HighPassFilter.Process(Buffer[i]), m_Tone);
	}

private:
	float m_Tone;
	LowPassFilter<T, SampleRate> m_LowPassFilter;
	HighPassFilter<T, SampleRate> m_HighPassFilter;
};

#endif