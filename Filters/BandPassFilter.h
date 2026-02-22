#pragma once
#ifndef BAND_PASS_FILTER_H
#define BAND_PASS_FILTER_H

#include "BiquadFilter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class BandPassFilter : private BiquadFilter<T, 1, SampleRate>
{
public:
	BandPassFilter(void)
		: m_CenterFrequency(1),
		  m_Bandwidth(1),
		  m_QualityFactory(1)
	{
		SetFrequencies(1950, 2050);
		SetQualityFactory(QUALITY_FACTOR_MAXIMALLY_FLAT);
	}

	//[MIN_FREQUENCY, MAX_FREQUENCY]
	void SetCenterFrequency(float Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

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
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

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
		ASSERT(MIN_FREQUENCY <= Min && Min <= MAX_FREQUENCY, "Invalid Min %f", Min);
		ASSERT(MIN_FREQUENCY <= Max && Max <= MAX_FREQUENCY, "Invalid Max %f", Max);

		m_Bandwidth = Max - Min;
		m_CenterFrequency = Math::FrequencyLerp(Min, Max, 0.5);

		Update();
	}

	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetQualityFactory(float Value)
	{
		ASSERT(QUALITY_FACTOR_MINIMUM <= Value && Value <= QUALITY_FACTOR_MAXIMUM, "Invalid Value %f", Value);

		m_QualityFactory = Value;

		Update();
	}
	float GetQualityFactory(void) const
	{
		return m_QualityFactory;
	}

	T Process(T Value) override
	{
		return BiquadFilter<T, 1, SampleRate>::Process(Value);
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1, SampleRate>::SetBandPassFilterCoefficients(this, m_CenterFrequency, m_Bandwidth, m_QualityFactory);
	}

private:
	float m_CenterFrequency;
	float m_Bandwidth;
	float m_QualityFactory;
};

#endif