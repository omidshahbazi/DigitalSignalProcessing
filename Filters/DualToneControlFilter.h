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
		SetBorderFrequencies(1 * KHz, 5);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	//[0, (LowHigh / 10) - (MIN_FREQUENCY / 10)]
	void SetBorderFrequencies(float LowHigh, float Threshold = 5)
	{
		const float LowHighThreshold = Threshold * 10;

		ASSERT(MIN_FREQUENCY <= LowHigh && LowHigh <= MAX_FREQUENCY, "Invalid LowHigh");
		ASSERT(MIN_FREQUENCY <= LowHigh - LowHighThreshold, "Invalid Threshold");

		m_LowPassFilter.SetCutoffFrequency(LowHigh);
		m_HighPassFilter.SetCutoffFrequency(LowHigh - LowHighThreshold);
	}

	//[-20dB, 20dB]
	void SetLowTone(float Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value");

		m_LowTone = Value;
		m_LowToneMultiplier = Math::dbToMultiplier(m_LowTone);
	}
	float GetLowTone(void) const
	{
		return m_LowTone;
	}

	//[-20dB, 20dB]
	void SetHighTone(float Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value");

		m_HighTone = Value;
		m_MidToneMultiplier = Math::dbToMultiplier(m_MidTone);
	}
	float GetHighTone(void) const
	{
		return m_HighTone;
	}

	T Process(T Value) override
	{
		return (m_LowPassFilter.Process(Value) * m_LowToneMultiplier) +
			   (m_HighPassFilter.Process(Value) * m_HighToneMultiplier);
	}

private:
	float m_LowTone;
	float m_HighTone;

	LowPassFilter<T, SampleRate> m_LowPassFilter;
	HighPassFilter<T, SampleRate> m_HighPassFilter;

	float m_LowToneMultiplier;
	float m_HighToneMultiplier;
};
#endif