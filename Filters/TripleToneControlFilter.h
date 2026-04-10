#pragma once
#ifndef TRIPLE_TONE_CONTROL_FILTER_H
#define TRIPLE_TONE_CONTROL_FILTER_H

#include "../Math.h"
#include "LowShelfFilter.h"
#include "PeakEQFilter.h"
#include "HighShelfFilter.h"

template <typename T, uint32 SampleRate>
class TripleToneControlFilter : public Filter<T, SampleRate>
{
public:
	TripleToneControlFilter(void)
	{
		SetBorderFrequencies(Frequency(150), Frequency(5 KHz));
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBorderFrequencies(Frequency LowMid, Frequency MidHigh)
	{
		ASSERT(MIN_FREQUENCY <= LowMid && LowMid <= MAX_FREQUENCY, "Invalid LowMid %f", LowMid);
		ASSERT(MIN_FREQUENCY <= MidHigh && MidHigh <= MAX_FREQUENCY, "Invalid MidHigh %f", MidHigh);

		m_LowShelfFilter.SetCutoffFrequency(LowMid);

		m_PeakEQFilter.SetBand(LowMid, MidHigh);

		m_HighShelfFilter.SetCutoffFrequency(MidHigh);
	}

	//[-12dB, 12dB]
	void SetLowTone(dBGain Value)
	{
		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_LowShelfFilter.SetGain(Value);
	}
	dBGain GetLowTone(void) const
	{
		return m_LowShelfFilter.GetGain();
	}

	//[-12dB, 12dB]
	void SetMidTone(dBGain Value)
	{
		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_PeakEQFilter.SetGain(Value);
	}
	dBGain GetMidTone(void) const
	{
		return m_PeakEQFilter.GetGain();
	}

	//[-12dB, 12dB]
	void SetHighTone(dBGain Value)
	{
		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_HighShelfFilter.SetGain(Value);
	}
	dBGain GetHighTone(void) const
	{
		return m_HighShelfFilter.GetGain();
	}

	void Process(T *Buffer, uint8 Count) override
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