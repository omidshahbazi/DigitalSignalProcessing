#pragma once
#ifndef TRIPLE_TONE_CONTROL_FILTER_H
#define TRIPLE_TONE_CONTROL_FILTER_H

#include "../Math.h"
#include "LowPassFilter.h"
#include "BandPassFilter.h"
#include "HighPassFilter.h"

template <typename T, uint32 SampleRate>
class TripleToneControlFilter : public Filter<T, SampleRate>
{
public:
	TripleToneControlFilter(void)
		: m_LowTone(0),
		  m_MidTone(0),
		  m_HighTone(0),
		  m_LowToneMultiplier(0),
		  m_MidToneMultiplier(0),
		  m_HighToneMultiplier(0)

	{
		SetBorderFrequencies(150, 5 * KHz);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	//[0, (LowMid / 10) - (MIN_FREQUENCY / 10)]
	void SetBorderFrequencies(float LowMid, float MidHigh, float Threshold = 5)
	{
		const float LowMidThreshold = Threshold * 10;
		const float MidHighThreshold = (Threshold * 0.1) * KHz;

		ASSERT(MIN_FREQUENCY <= LowMid && LowMid <= MAX_FREQUENCY, "Invalid LowMid %f", LowMid);
		ASSERT(MIN_FREQUENCY <= MidHigh && MidHigh <= MAX_FREQUENCY, "Invalid MidHigh %f", MidHigh);
		ASSERT(MIN_FREQUENCY <= LowMid - LowMidThreshold, "Invalid Threshold %f", Threshold);

		m_LowPassFilter.SetCutoffFrequency(LowMid);
		m_BandPassFilter.SetFrequencies(LowMid - LowMidThreshold, MidHigh);
		m_HighPassFilter.SetCutoffFrequency(MidHigh - MidHighThreshold);
	}

	//[-20dB, 20dB]
	void SetLowTone(float Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_LowTone = Value;
		m_LowToneMultiplier = Math::dBToLinear(m_LowTone);
	}
	float GetLowTone(void) const
	{
		return m_LowTone;
	}

	//[-20dB, 20dB]
	void SetMidTone(float Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_MidTone = Value;
		m_MidToneMultiplier = Math::dBToLinear(m_MidTone);
	}
	float GetMidTone(void) const
	{
		return m_MidTone;
	}

	//[-20dB, 20dB]
	void SetHighTone(float Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_HighTone = Value;
		m_HighToneMultiplier = Math::dBToLinear(m_HighTone);
	}
	float GetHighTone(void) const
	{
		return m_HighTone;
	}

	T Process(T Value) override
	{
		return (m_LowPassFilter.Process(Value) * m_LowToneMultiplier) +
			   (m_BandPassFilter.Process(Value) * m_MidToneMultiplier) +
			   (m_HighPassFilter.Process(Value) * m_HighToneMultiplier);
	}

private:
	float m_LowTone;
	float m_MidTone;
	float m_HighTone;

	LowPassFilter<T, SampleRate> m_LowPassFilter;
	BandPassFilter<T, SampleRate> m_BandPassFilter;
	HighPassFilter<T, SampleRate> m_HighPassFilter;

	float m_LowToneMultiplier;
	float m_MidToneMultiplier;
	float m_HighToneMultiplier;
};
#endif