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
		: m_CenterFrequency(MIN_FREQUENCY),
		  m_Bandwidth(MIN_FREQUENCY),
		  m_QualityFactor(QUALITY_FACTOR_MAXIMALLY_FLAT)
	{
		Update();
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
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

	// [MIN_FREQUENCY, MAX_FREQUENCY]
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

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	void SetFrequencies(float Min, float Max)
	{
		ASSERT(MIN_FREQUENCY <= Min && Min <= MAX_FREQUENCY, "Invalid Min %f", Min);
		ASSERT(MIN_FREQUENCY <= Max && Max <= MAX_FREQUENCY, "Invalid Max %f", Max);

		m_Bandwidth = Max - Min;
		m_CenterFrequency = Math::FrequencyLerp(Min, Max, 0.5);

		Update();
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

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(float CenterFrequency, float Bandwidth, float QualityFactor)
	{
		ASSERT(MIN_FREQUENCY <= CenterFrequency && CenterFrequency <= MAX_FREQUENCY, "Invalid CenterFrequency %f", CenterFrequency);
		ASSERT(MIN_FREQUENCY <= Bandwidth && Bandwidth <= MAX_FREQUENCY, "Invalid Bandwidth %f", Bandwidth);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);
		
		m_CenterFrequency = CenterFrequency;
		m_Bandwidth = Bandwidth;
		m_QualityFactor = QualityFactor;

		Update();
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParametersRange(float MinFrequency, float MaxFrequency, float QualityFactor)
	{
		ASSERT(MIN_FREQUENCY <= MinFrequency && MinFrequency <= MAX_FREQUENCY, "Invalid MinFrequency %f", MinFrequency);
		ASSERT(MIN_FREQUENCY <= MaxFrequency && MaxFrequency <= MAX_FREQUENCY, "Invalid MaxFrequency %f", MaxFrequency);
		ASSERT(QUALITY_FACTOR_MINIMUM <= QualityFactor && QualityFactor <= QUALITY_FACTOR_MAXIMUM, "Invalid QualityFactor %f", QualityFactor);
		
		m_CenterFrequency = Math::FrequencyLerp(MinFrequency, MaxFrequency, 0.5);
		m_Bandwidth = MaxFrequency - MinFrequency;
		m_QualityFactor = QualityFactor;

		Update();
	}

	void Process(T *Buffer, uint8 Count) override
	{
		return BiquadFilter<T, 1, SampleRate>::Process(Buffer, Count);
	}

	T Process(T Value) override
	{
		BiquadFilter<T, 1, SampleRate>::Process(&Value, 1);

		return Value;
	}

private:
	void Update(void)
	{
		BiquadFilter<T, 1, SampleRate>::SetBandPassFilterCoefficients(this, m_CenterFrequency, m_Bandwidth, m_QualityFactor);
	}

private:
	float m_CenterFrequency;
	float m_Bandwidth;
	float m_QualityFactor;
};

#endif