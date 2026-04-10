#pragma once
#ifndef WAH_H
#define WAH_H

#include "IDSP.h"
#include "../Filters/PeakEQFilter.h"

template <typename T, uint32 SampleRate>
class Wah : public IDSP<T, SampleRate>
{
private:
	struct FrequencyRange
	{
	public:
		float FrequencyMin;
		float FrequencyMax;
		float QualityFactorMin;
		float QualityFactorMax;
		float GainMin;
		float GainMax;
		float FinalGain;
	};

	const FrequencyRange FREQUENCY_RANGES[3] = {
		{400, 2.1 KHz, 2, 4.5, 12, 16, 0.6},  // Classic
		{300, 1.2 KHz, 2, 4.5, 10, 14, 0.5},  // Deep
		{500, 3.0 KHz, 2, 4.5, 14, 20, 0.4}}; // Sharp

public:
	enum class Types
	{
		Classic = 0,
		Deep,
		Sharp
	};

public:
	Wah(void)
		: m_Ratio(0)
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

		m_Ratio = Math::Lerp(m_Ratio, Value, 0.01);

		const FrequencyRange &freqRange = FREQUENCY_RANGES[(uint32)m_Type];
		Frequency freq = (Frequency)Math::FrequencyLerp(freqRange.FrequencyMin, freqRange.FrequencyMax, m_Ratio);
		QualityFactor q = (QualityFactor)Math::Lerp(freqRange.QualityFactorMin, freqRange.QualityFactorMax, m_Ratio);
		dBGain gain = (dBGain)Math::Lerp(freqRange.GainMin, freqRange.GainMax, m_Ratio);

		m_PeakEQFilter.SetParameters(FrequencyBand(freq, q), gain);
	}
	float GetRatio(void) const
	{
		return m_Ratio;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		m_PeakEQFilter.Process(Buffer, Count);

		const FrequencyRange &freqRange = FREQUENCY_RANGES[(uint32)m_Type];
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::SoftClip(Buffer[i]) * freqRange.FinalGain;
	}

private:
	PeakEQFilter<T, SampleRate> m_PeakEQFilter;
	Types m_Type;
	float m_Ratio;
};

#endif