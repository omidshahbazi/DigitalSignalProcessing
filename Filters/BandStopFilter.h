#pragma once
#ifndef BAND_STOP_FILTER_H
#define BAND_STOP_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class BandStopFilter : private BiquadFilter<T, 1, SampleRate>
{
public:
	BandStopFilter(void)
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
		m_CenterFrequency = Math::FrequencyLerp(Min, Max, 0.5);

		Update();
	}

	// [0.1, 10]
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
		return BiquadFilter<T, 1, SampleRate>::Process(Value);
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1, SampleRate>::SetBandStopFilterCoefficients(this, m_CenterFrequency, m_Bandwidth, m_Resonance);
	}

private:
	float m_CenterFrequency;
	float m_Bandwidth;
	float m_Resonance;
};

#endif