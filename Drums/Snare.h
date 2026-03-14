#pragma once
#ifndef SNARE_H
#define SNARE_H

#include "DrumsPart.h"
#include "../Filters/AttackDecaySustainReleaseEnvelopeFilter.h"
#include "../Filters/WhiteNoiseFilter.h"
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

		m_BandPass.SetFrequencies(8 KHz, 10 KHz);
		m_BandPass.SetQualityFactor(3);
	}

	void Beat(void) override
	{
		m_Envelope.Trigger();
	}

	T Process(void) override
	{
		return m_BandPass.Process(m_WhiteNoiseFilter.Process()) * m_Envelope.Process();
	}

private:
	AttackDecaySustainReleaseEnvelopeFilter<T, SampleRate> m_Envelope;
	WhiteNoiseFilter<T, SampleRate> m_WhiteNoiseFilter;
	BandPassFilter<T, SampleRate> m_BandPass;
};

#endif