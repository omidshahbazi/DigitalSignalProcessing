#pragma once
#ifndef SNARE_H
#define SNARE_H

#include "DrumsPart.h"
#include "../Filters/AttackDecaySustainReleaseEnvelopeFilter.h"
#include "../Filters/WhiteNoiseFilter.h"
#include "../Filters/MetalNoiseFilter.h"
#include "../Filters/BandPassFilter.h"

template <typename T, uint32 SampleRate>
class Snare : public DrumsPart<T, SampleRate>
{
public:
	Snare(void)
	{
		m_Envelope.SetAttackTime(10 ms);
		m_Envelope.SetDecayTime(200 ms);
		m_Envelope.SetSustainTime(100 ns);
		m_Envelope.SetReleaseTime(300 ms);
		m_Envelope.SetMinValue(0);
		m_Envelope.SetMaxValue(1);

		m_BandPass.SetBand(Frequency(8 KHz), Frequency(10 KHz));
	}

	void Beat(void) override
	{
		m_Envelope.Trigger();
	}

	T Process(void) override
	{
		T sample = m_WhiteNoiseFilter.Process() + m_MetalNoiseFilter.Process();

		return m_BandPass.Process(sample) * m_Envelope.Process();
	}

private:
	AttackDecaySustainReleaseEnvelopeFilter<T, SampleRate> m_Envelope;
	WhiteNoiseFilter<T, SampleRate> m_WhiteNoiseFilter;
	MetalNoiseFilter<T, SampleRate> m_MetalNoiseFilter;
	BandPassFilter<T, SampleRate> m_BandPass;
};

#endif