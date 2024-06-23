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
		m_Envelope.SetAttackTime(0.01);
		m_Envelope.SetDecayTime(0.2);
		m_Envelope.SetSustainTime(0.0001);
		m_Envelope.SetReleaseTime(0.3);
		m_Envelope.SetMinValue(0);
		m_Envelope.SetMaxValue(1);

		m_BandPass.SetFrequencies(8 * KHz, 10 * KHz);
		m_BandPass.SetResonance(3);
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