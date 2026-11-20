#pragma once
#ifndef DUAL_TONE_CONTROL_FILTER_H
#define DUAL_TONE_CONTROL_FILTER_H

#include "DigitalSignalProcessing/Math.h"
#include "LowShelfFilter.h"
#include "HighShelfFilter.h"

template <typename T, uint32_t SampleRate>
class DualToneControlFilter : public Filter<T, SampleRate>
{
public:
	DualToneControlFilter(void)
	{
		SetBorderFrequency(1 KHz);
	}

	void SetBorderFrequency(Frequency_hearable Value)
	{
		m_LowShelfFilter.SetCutoffFrequency(Value);
		m_HighShelfFilter.SetCutoffFrequency(Value);
	}

	void SetLowTone(dBGain_12 Value)
	{
		m_LowTone = Value;

		m_LowShelfFilter.SetGain(m_LowTone);
	}
	dBGain GetLowTone(void) const
	{
		return m_LowTone;
	}

	void SetHighTone(dBGain_12 Value)
	{
		m_HighTone = Value;

		m_HighShelfFilter.SetGain(m_HighTone);
	}
	dBGain GetHighTone(void) const
	{
		return m_HighTone;
	}

	void Process(T *Buffer, uint8_t Count) override
	{
		m_LowShelfFilter.Process(Buffer, Count);
		m_HighShelfFilter.Process(Buffer, Count);
	}

private:
	LowShelfFilter<T, SampleRate> m_LowShelfFilter;
	HighShelfFilter<T, SampleRate> m_HighShelfFilter;
};
#endif