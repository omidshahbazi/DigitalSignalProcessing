#pragma once
#ifndef HIGH_SHELF_FILTER_H
#define HIGH_SHELF_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class HighShelfFilter : private BiquadFilter<T, 1, SampleRate>
{
public:
	HighShelfFilter(void)
		: m_CutoffFrequency(MIN_FREQUENCY),
		  m_Gain(0),
		  m_SlopeFactor(SLOPE_FACTOR_MINIMUM)
	{
		Update();
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetCutoffFrequency(float Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_CutoffFrequency = Value;

		Update();
	}
	float GetCutoffFrequency(void) const
	{
		return m_CutoffFrequency;
	}

	// [-20dB, 20dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_Gain = Value;

		Update();
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	// [SLOPE_FACTOR_MINIMUM, SLOPE_FACTOR_MAXIMUM]
	void SetSlopeFactor(float Value)
	{
		ASSERT(SLOPE_FACTOR_MINIMUM <= Value && Value <= SLOPE_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_SlopeFactor = Value;

		Update();
	}
	float GetSlopeFactor(void) const
	{
		return m_SlopeFactor;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		return BiquadFilter<T, 1, SampleRate>::Process(Buffer, Count);
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1, SampleRate>::SetHighShelfCoefficients(this, m_CutoffFrequency, m_Gain, m_SlopeFactor);
	}

private:
	float m_CutoffFrequency;
	float m_Gain;
	float m_SlopeFactor;
};

#endif