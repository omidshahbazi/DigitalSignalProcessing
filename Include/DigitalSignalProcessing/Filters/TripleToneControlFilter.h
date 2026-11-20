#pragma once
#ifndef TRIPLE_TONE_CONTROL_FILTER_H
#define TRIPLE_TONE_CONTROL_FILTER_H

#include "DigitalSignalProcessing/Math.h"
#include "LowShelfFilter.h"
#include "PeakEQFilter.h"
#include "HighShelfFilter.h"

template <typename T, uint32_t SampleRate>
class TripleToneControlFilter : public Filter<T, SampleRate>
{
public:
	TripleToneControlFilter(void)
	{
		SetBorderFrequencies(Frequency(150), Frequency(5 KHz));
	}

	void SetBorderFrequencies(Frequency_hearable LowMid, Frequency_hearable MidHigh)
	{
		m_LowShelfFilter.SetCutoffFrequency(LowMid);

		m_PeakEQFilter.SetBand(LowMid, MidHigh);

		m_HighShelfFilter.SetCutoffFrequency(MidHigh);
	}

	void SetLowTone(dBGain_12 Value)
	{
		m_LowShelfFilter.SetGain((dBGain)Value);
	}
	dBGain GetLowTone(void) const
	{
		return m_LowShelfFilter.GetGain();
	}

	void SetMidTone(dBGain_12 Value)
	{
		m_PeakEQFilter.SetGain((dBGain)Value);
	}
	dBGain GetMidTone(void) const
	{
		return m_PeakEQFilter.GetGain();
	}

	void SetHighTone(dBGain_12 Value)
	{
		m_HighShelfFilter.SetGain((dBGain)Value);
	}
	dBGain GetHighTone(void) const
	{
		return m_HighShelfFilter.GetGain();
	}

	void Process(T *Buffer, uint8_t Count) override
	{
		m_LowShelfFilter.Process(Buffer, Count);
		m_PeakEQFilter.Process(Buffer, Count);
		m_HighShelfFilter.Process(Buffer, Count);
	}

private:
	LowShelfFilter<T, SampleRate> m_LowShelfFilter;
	PeakEQFilter<T, SampleRate> m_PeakEQFilter;
	HighShelfFilter<T, SampleRate> m_HighShelfFilter;
};
#endif