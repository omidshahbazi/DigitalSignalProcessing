#pragma once
#ifndef DUAL_TONE_CONTROL_FILTER_H
#define DUAL_TONE_CONTROL_FILTER_H

#include "../Math.h"
#include "LowShelfFilter.h"
#include "HighShelfFilter.h"

template <typename T, uint32 SampleRate>
class DualToneControlFilter : public Filter<T, SampleRate>
{
public:
	DualToneControlFilter(void)
	{
		SetBorderFrequency(1 KHz);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBorderFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_LowShelfFilter.SetCutoffFrequency(Value);
		m_HighShelfFilter.SetCutoffFrequency(Value);
	}

	//[-12dB, 12dB]
	void SetLowTone(dBGain Value)
	{
		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_LowTone = Value;

		m_LowShelfFilter.SetGain(m_LowTone);
	}
	dBGain GetLowTone(void) const
	{
		return m_LowTone;
	}

	//[-12dB, 12dB]
	void SetHighTone(dBGain Value)
	{
		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_HighTone = Value;

		m_HighShelfFilter.SetGain(m_HighTone);
	}
	dBGain GetHighTone(void) const
	{
		return m_HighTone;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		m_LowShelfFilter.Process(Buffer, Count);
		m_HighShelfFilter.Process(Buffer, Count);
	}

private:
	LowShelfFilter<T, SampleRate, 1> m_LowShelfFilter;
	HighShelfFilter<T, SampleRate, 1> m_HighShelfFilter;
};
#endif