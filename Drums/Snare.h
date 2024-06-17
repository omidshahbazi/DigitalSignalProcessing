#pragma once
#ifndef SNARE_H
#define SNARE_H

#include "DrumsPart.h"
#include "../Filters/AttackDecayEnvelopeFilter.h"
#include "../Filters/WhiteNoiseFilter.h"

template <typename T, uint32 SampleRate>
class Snare : public DrumsPart<T, SampleRate>
{
public:
	Snare(void)
	{
		m_SnareEnvelope.SetAttackTime(0.01);
		m_SnareEnvelope.SetDecayTime(0.2);
		m_SnareEnvelope.SetMaxValue(1);
		m_SnareEnvelope.SetMinValue(0);
	}

	void Beat(void) override
	{
		m_SnareEnvelope.Trigger();
	}

	T Process(void) override
	{
		return m_WhiteNoiseFilter.Process() * m_SnareEnvelope.Process();
	}

private:
	AttackDecayEnvelopeFilter<T, SampleRate> m_SnareEnvelope;
	WhiteNoiseFilter<T, SampleRate> m_WhiteNoiseFilter;
};

#endif