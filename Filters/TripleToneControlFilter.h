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
		SetBorderFrequencies(150, 5 KHz);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBorderFrequencies(float LowMid, float MidHigh, Octave Offset = ONE_OCTAVE)
	{
		const float LowFreq = LowMid / Offset;
		const float HighFreq = MidHigh * Offset;
		const float MidFreq = Math::FrequencyLerp(LowFreq, HighFreq, 0.5);

		ASSERT(MIN_FREQUENCY <= LowFreq && LowFreq <= MAX_FREQUENCY, "Invalid LowFreq %f", LowFreq);
		ASSERT(MIN_FREQUENCY <= MidFreq && MidFreq <= MAX_FREQUENCY, "Invalid MidFreq %f", MidFreq);
		ASSERT(MIN_FREQUENCY <= HighFreq && HighFreq <= MAX_FREQUENCY, "Invalid HighFreq %f", HighFreq);

		m_LowShelfFilter.SetCutoffFrequency(LowFreq);
		
		m_PeakEQFilter.SetQualityFactor(BiquadFilter<T, 1, SampleRate>::CalculateCoveringQ(LowMid, MidHigh));
		m_PeakEQFilter.SetCutoffFrequency(MidFreq);

		m_HighShelfFilter.SetCutoffFrequency(HighFreq);
	}

	//[-20dB, 40dB]
	void SetLowTone(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 40, "Invalid Value %f", Value);

		m_LowShelfFilter.SetGain(Value);
	}
	dBGain GetLowTone(void) const
	{
		return m_LowShelfFilter.GetGain();
	}

	//[-20dB, 40dB]
	void SetMidTone(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 40, "Invalid Value %f", Value);

		m_PeakEQFilter.SetGain(Value);
	}
	dBGain GetMidTone(void) const
	{
		return m_PeakEQFilter.GetGain();
	}

	//[-20dB, 40dB]
	void SetHighTone(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 40, "Invalid Value %f", Value);

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