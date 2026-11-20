#pragma once
#ifndef WAH_H
#define WAH_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Filters/PeakEQFilter.h"

template <typename T, uint32_t SampleRate>
class Wah : public IDSP<T, SampleRate>
{
private:
	struct FrequencyRange
	{
	public:
		Frequency FrequencyMin;
		Frequency FrequencyMax;
		QualityFactor QualityFactorMin;
		QualityFactor QualityFactorMax;
		dBGain GainMin;
		dBGain GainMax;
		dBGain FinalGain;
	};

	const FrequencyRange FREQUENCY_RANGES[3] = {
		{Frequency(400), Frequency(2.1 KHz), QualityFactor(2), QualityFactor(4.5), dBGain(12), dBGain(16), dBGain(0.6)},  // Classic
		{Frequency(300), Frequency(1.2 KHz), QualityFactor(2), QualityFactor(4.5), dBGain(10), dBGain(14), dBGain(0.5)},  // Deep
		{Frequency(500), Frequency(3.0 KHz), QualityFactor(2), QualityFactor(4.5), dBGain(14), dBGain(20), dBGain(0.4)} }; // Sharp

public:
	enum class Types
	{
		Classic = 0,
		Deep,
		Sharp
	};

public:
	Wah(void)
		: m_Ratio(1)
	{
		SetType(Types::Classic);
	}

	void SetType(Types Value)
	{
		m_Type = Value;

		UpdateParameters();
	}
	Types GetType(void) const
	{
		return m_Type;
	}

	void SetRatio(cc_ratio_t Value)
	{
		m_Ratio = Value;

		UpdateParameters();
	}
	float GetRatio(void) const
	{
		return m_Ratio;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		m_PeakEQFilter.Process(Buffer, Count);

		const FrequencyRange& freqRange = FREQUENCY_RANGES[(uint32_t)m_Type];
		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= freqRange.FinalGain;
	}

private:
	void UpdateParameters(void)
	{
		const FrequencyRange& freqRange = FREQUENCY_RANGES[(uint32_t)m_Type];
		Frequency freq = (Frequency)Math::FrequencyLerp(freqRange.FrequencyMin, freqRange.FrequencyMax, m_Ratio);
		QualityFactor q = (QualityFactor)Math::Lerp(freqRange.QualityFactorMin, freqRange.QualityFactorMax, m_Ratio);
		dBGain gain = (dBGain)Math::Lerp(freqRange.GainMin, freqRange.GainMax, m_Ratio);

		m_PeakEQFilter.SetParameters(FrequencyBand(freq, q), gain);
	}

private:
	PeakEQFilter<T, SampleRate> m_PeakEQFilter;
	Types m_Type;
	float m_Ratio;
};

#endif