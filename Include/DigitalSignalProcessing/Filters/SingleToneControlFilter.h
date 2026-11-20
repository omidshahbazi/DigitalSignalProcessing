#pragma once
#ifndef SINGLE_TONE_CONTROL_FILTER_H
#define SINGLE_TONE_CONTROL_FILTER_H

#include "DigitalSignalProcessing/Math.h"
#include "LowShelfFilter.h"
#include "HighShelfFilter.h"

template <typename T, uint32_t SampleRate>
class SingleToneControlFilter : public Filter<T, SampleRate>
{
public:
	SingleToneControlFilter(void)
		: m_Tone(0)
	{
		SetTone(0.5);
		SetBorderFrequency((Frequency)(1 KHz));
	}

	void SetBorderFrequency(Frequency_hearable Value)
	{
		m_LowShelfFilter.SetCutoffFrequency(Value);
		m_HighShelfFilter.SetCutoffFrequency(Value);
	}

	void SetTone(cc_ratio_t Value)
	{
		m_Tone = Value;

		const dBGain Gain(6);

		m_LowShelfFilter.SetGain((dBGain)Math::Map(1 - m_Tone, 0.0F, 1, -Gain, Gain));
		m_HighShelfFilter.SetGain((dBGain)Math::Map(m_Tone, 0.0F, 1, -Gain, Gain));
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	void Process(T *Buffer, uint8_t Count) override
	{
		m_LowShelfFilter.Process(Buffer, Count);
		m_HighShelfFilter.Process(Buffer, Count);
	}

private:
	float m_Tone;

	LowShelfFilter<T, SampleRate> m_LowShelfFilter;
	HighShelfFilter<T, SampleRate> m_HighShelfFilter;
};

#endif