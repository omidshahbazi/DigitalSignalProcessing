#pragma once
#ifndef DUAL_TONE_CONTROL_FILTER_H
#define DUAL_TONE_CONTROL_FILTER_H

#include "../Math.h"
#include "LowPassFilter.h"
#include "BandPassFilter.h"
#include "HighPassFilter.h"

template <typename T, uint32 SampleRate>
class DualToneControlFilter : public Filter<T, SampleRate>
{
public:
	DualToneControlFilter(void)
		: m_LowTone(0),
		  m_HighTone(0),
		  m_LowToneMultiplier(0),
		  m_HighToneMultiplier(0)
	{
		SetBorderFrequencies(1 KHz, 5);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	//[0, (LowHigh / 10) - (MIN_FREQUENCY / 10)]
	void SetBorderFrequencies(float LowHigh, float Threshold = 5)
	{
		const float LowHighThreshold = Threshold * 10;

		ASSERT(MIN_FREQUENCY <= LowHigh && LowHigh <= MAX_FREQUENCY, "Invalid LowHigh %f", LowHigh);
		ASSERT(MIN_FREQUENCY <= LowHigh - LowHighThreshold, "Invalid Threshold %f", Threshold);

		m_LowPassFilter.SetCutoffFrequency(LowHigh);
		m_HighPassFilter.SetCutoffFrequency(LowHigh - LowHighThreshold);
	}

	//[-20dB, 20dB]
	void SetLowTone(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_LowTone = Value;
		m_LowToneMultiplier = m_LowTone;
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
		m_MidToneMultiplier = m_MidTone;
	}
	dBGain GetHighTone(void) const
	{
		return m_HighTone;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Process(Buffer[i]);
	}
	
	T Process(T Value)
	{
		return (m_LowPassFilter.Process(Value) * m_LowToneMultiplier) +
			   (m_HighPassFilter.Process(Value) * m_HighToneMultiplier);
	}

private:
	dBGain m_LowTone;
	dBGain m_HighTone;

	LowPassFilter<T, SampleRate> m_LowPassFilter;
	HighPassFilter<T, SampleRate> m_HighPassFilter;

	float m_LowToneMultiplier;
	float m_HighToneMultiplier;
};
#endif