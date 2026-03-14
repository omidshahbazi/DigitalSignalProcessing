#pragma once
#ifndef PEAK_EQ_FILTER_H
#define PEAK_EQ_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class PeakEQFilter : private BiquadFilter<T, 1, SampleRate>
{
public:
	PeakEQFilter(void)
		: m_CutoffFrequency(MIN_FREQUENCY),
		  m_Gain(0),
		  m_QualityFactor(QUALITY_FACTOR_MAXIMALLY_FLAT)
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
	void SetGain(float Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_Gain = Value;

		Update();
	}
	float GetGain(void) const
	{
		return m_Gain;
	}

	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetQualityFactor(float Value)
	{
		ASSERT(QUALITY_FACTOR_MINIMUM <= Value && Value <= QUALITY_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_QualityFactor = Value;

		Update();
	}
	float GetQualityFactor(void) const
	{
		return m_QualityFactor;
	}

	T Process(T Value) override
	{
		return BiquadFilter<T, 1, SampleRate>::Process(Value);
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1, SampleRate>::SetPeakEQCoefficients(this, m_CutoffFrequency, m_Gain, m_QualityFactor);
	}

private:
	float m_CutoffFrequency;
	float m_Gain;
	float m_QualityFactor;
};

#endif