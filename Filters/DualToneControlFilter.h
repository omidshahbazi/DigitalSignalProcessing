#pragma once
#ifndef DUAL_TONE_CONTROL_FILTER_H
#define DUAL_TONE_CONTROL_FILTER_H

#include "../Math.h"
#include "LowPassFilter.h"
#include "BandPassFilter.h"
#include "HighPassFilter.h"

template <typename T>
class DualToneControlFilter : public Filter<T>
{
#define MULTIPLIER 0.0495

public:
	DualToneControlFilter(uint32 SampleRate)
		: m_LowTone(0),
		  m_HighTone(0),
		  m_LowPassFilter(SampleRate),
		  m_HighPassFilter(SampleRate)
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
	}
	float GetHighTone(void) const
	{
		return m_HighTone;
	}

	T Process(T Value) override
	{
		return (m_LowPassFilter.Process(Value) * (1 + (m_LowTone * MULTIPLIER))) +
			   (m_HighPassFilter.Process(Value) * (1 + (m_HighTone * MULTIPLIER)));
	}

private:
	float m_LowTone;
	float m_HighTone;

	LowPassFilter<T> m_LowPassFilter;
	HighPassFilter<T> m_HighPassFilter;
};
#endif