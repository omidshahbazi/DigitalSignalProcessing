#pragma once
#ifndef BAND_PASS_FILTER_H
#define BAND_PASS_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class BandPassFilter : private BiquadFilter<T, 1>
{
public:
	BandPassFilter(void)
		: m_CenterFrequency(1),
		  m_Bandwidth(1),
		  m_Resonance(1)
	{
		SetFrequencies(1950, 2050);
		SetResonance(1);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetCenterFrequency(float Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value");

		m_CenterFrequency = Value;

		Update();
	}
	float GetCenterFrequency(void) const
	{
		return m_CenterFrequency;
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetBandwidth(float Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value");

		m_Bandwidth = Value;

		Update();
	}
	float GetBandwidth(void) const
	{
		return m_Bandwidth;
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetFrequencies(float Min, float Max)
	{
		ASSERT(MIN_FREQUENCY <= Min && Min <= MAX_FREQUENCY, "Invalid Min");
		ASSERT(MIN_FREQUENCY <= Max && Max <= MAX_FREQUENCY, "Invalid Max");

		m_Bandwidth = Max - Min;
		m_CenterFrequency = Min + (m_Bandwidth / 2);

		Update();
	}

	// [0.1, 10] Common [0.7, 1]
	// - Values less than 0.7 result in a broader bandwidth and less peak resonance, making the filter less selective.
	// - Values greater than 1 increase resonance, creating a sharper peak around the center frequency, which can lead to more pronounced resonance or oscillations.
	void SetResonance(float Value)
	{
		ASSERT(0.1 <= Value && Value <= 10, "Invalid Value");

		m_Resonance = Value;

		Update();
	}
	float GetResonance(void) const
	{
		return m_Resonance;
	}

	T Process(T Value) override
	{
		return BiquadFilter<T, 1>::Process(Value);
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1>::SetBandPassFilterCoefficients(this, SampleRate, m_CenterFrequency, m_Bandwidth, m_Resonance);
	}

private:
	float m_CenterFrequency;
	float m_Bandwidth;
	float m_Resonance;
};

#endif