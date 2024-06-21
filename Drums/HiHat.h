#pragma once
#ifndef HI_HAT_H
#define HI_HAT_H

#include "DrumsPart.h"
#include "../Filters/AttackDecayEnvelopeFilter.h"
#include "../Filters/WhiteNoiseFilter.h"

template <typename T, uint32 SampleRate>
class HiHat : public DrumsPart<T, SampleRate>
{
public:
	HiHat(void)
	{
		m_Envelope.SetAttackTime(0.005);
		m_Envelope.SetDecayTime(0.1);
		m_Envelope.SetMaxValue(1);
		m_Envelope.SetMinValue(0);
	}

	void Beat(void) override
	{
		m_Envelope.Trigger();
	}

	T Process(void) override
	{
		return m_WhiteNoiseFilter.Process() * m_Envelope.Process();
	}

private:
	AttackDecayEnvelopeFilter<T, SampleRate> m_Envelope;
	WhiteNoiseFilter<T, SampleRate> m_WhiteNoiseFilter;
};

#endif