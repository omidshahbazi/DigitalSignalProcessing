#pragma once
#ifndef PEAK_EQ_FILTER_H
#define PEAK_EQ_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate>
class PeakEQFilter : public BiquadBandBasedFilter<T, 1, SampleRate>
{
private:
	typedef BiquadBandBasedFilter<T, 1, SampleRate> Base;

public:
	PeakEQFilter(void)
		: m_Gain(0)
	{
	}

	// [-20dB, 20dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_Gain = Value;

		UpdateCoefficients();
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [-20dB, 20dB]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(FrequencyBand Band, dBGain Gain)
	{
		ASSERT(MIN_FREQUENCY <= Band.GetCenter() && Band.GetCenter() <= MAX_FREQUENCY, "Invalid Cutoff %f", Band.GetCenter());
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);

		Base::m_Band = Band;
		m_Gain = Gain;

		UpdateCoefficients();
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [OCTAVE_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(Frequency Center, LogarithmicOctave Bandwidth, dBGain Gain)
	{
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);

		m_Gain = Gain;
		Base::SetParameters(Center, Bandwidth);
	}

	// [MIN_FREQUENCY, MAX_FREQUENCY]
	// [QUALITY_FACTOR_MINIMUM, QUALITY_FACTOR_MAXIMUM]
	void SetParameters(Frequency Center, QualityFactor Quality, dBGain Gain)
	{
		ASSERT(-20 <= Gain && Gain <= 20, "Invalid Gain %f", Gain);
		
		m_Gain = Gain;
		Base::SetParameters(Center, Quality);
	}

private:
	void UpdateCoefficients(void) override
	{
		BiquadFilter<T, 1, SampleRate>::SetPeakEQCoefficients(this, Base::m_Band, m_Gain);
	}

private:
	dBGain m_Gain;
};

#endif