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
		SetBorderFrequencies(1 KHz);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBorderFrequency(float Value, Octave Offset = ONE_OCTAVE)
	{
		const float LowFreq = Value / Offset;
		const float HighFreq = Value * Offset;

		ASSERT(MIN_FREQUENCY <= LowFreq && LowFreq <= MAX_FREQUENCY, "Invalid Value %f", LowFreq);
		ASSERT(MIN_FREQUENCY <= HighFreq && HighFreq <= MAX_FREQUENCY, "Invalid Value %f", HighFreq);

		m_LowShelfFilter.SetCutoffFrequency(LowFreq);
		m_HighShelfFilter.SetCutoffFrequency(HighFreq);
	}

	//[-20dB, 20dB]
	void SetLowTone(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_LowTone = Value;

		m_LowShelfFilter.SetGain(m_LowTone);
	}
	dBGain GetLowTone(void) const
	{
		return m_LowTone;
	}

	//[-20dB, 20dB]
	void SetHighTone(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

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
	LowShelfFilter<T, SampleRate> m_LowShelfFilter;
	HighShelfFilter<T, SampleRate> m_HighShelfFilter;
};
#endif