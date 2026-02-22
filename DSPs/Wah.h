#pragma once
#ifndef WAH_H
#define WAH_H

#include "IDSP.h"
#include "../Filters/BandPassFilter.h"

template <typename T, uint32 SampleRate>
class Wah : public IDSP<T, SampleRate>
{
private:
	struct FrequencyRange
	{
	public:
		float FrequencyMin;
		float FrequencyMax;
		float QualityFactoryMin;
		float QualityFactoryMax;
	};

	const FrequencyRange FREQUENCY_RANGES[3] = {
		{400, 2.1 KHz, 2, 4.5},	// Classic
		{300, 1.2 KHz, 2, 4.5},	// Deep
		{500, 3.0 KHz, 2, 4.5}};	// Sharp

public:
	enum class Types
	{
		Classic = 0,
		Deep,
		Sharp
	};

public:
	Wah(void)
	{
		SetType(Types::Classic);
	}

	void SetType(Types Value)
	{
		m_Type = Value;

		SetRatio(m_Ratio);
	}
	Types GetType(void) const
	{
		return m_Type;
	}

	//[0, 1]
	void SetRatio(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Ratio = Value;

		const FrequencyRange &freqRange = FREQUENCY_RANGES[(uint32)m_Type];

		m_BandPassFilter.SetCenterFrequency(Math::FrequencyLerp(freqRange.FrequencyMin, freqRange.FrequencyMax, m_Ratio));
		m_BandPassFilter.SetQualityFactory(Math::Lerp(freqRange.QualityFactoryMin, freqRange.QualityFactoryMax, m_Ratio));
	}
	float GetRatio(void) const
	{
		return m_Ratio;
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = Process(Buffer[i]);
	}

protected:
	T Process(T Value)
	{
		return m_BandPassFilter.Process(Value) * 0.8;
	}

private:
	BandPassFilter<T, SampleRate> m_BandPassFilter;
	Types m_Type;
	float m_Ratio;
};

#endif