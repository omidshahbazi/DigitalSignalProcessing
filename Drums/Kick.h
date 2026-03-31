#pragma once
#ifndef KICK_H
#define KICK_H

#include "DrumsPart.h"
#include "../Filters/AttackDecaySustainReleaseEnvelopeFilter.h"
#include "../Filters/OscillatorFilter.h"
#include "../Filters/WhiteNoiseFilter.h"

template <typename T, uint32 SampleRate>
class Kick : public DrumsPart<T, SampleRate>
{
public:
	Kick(void)
	{
		m_Envelope.SetAttackTime(100 ns);
		m_Envelope.SetDecayTime(200 ms);
		m_Envelope.SetSustainTime(200 ms);
		m_Envelope.SetSustainLevel(1);
		m_Envelope.SetReleaseTime(400 ms);
		m_Envelope.SetMinValue(0);
		m_Envelope.SetMaxValue(1);

		m_FrequencyEnvelope.SetAttackTime(100 ns);
		m_FrequencyEnvelope.SetDecayTime(20 ms);
		m_FrequencyEnvelope.SetSustainTime(100 ns);
		m_FrequencyEnvelope.SetSustainLevel(1);
		m_FrequencyEnvelope.SetReleaseTime(20 ms);
		m_FrequencyEnvelope.SetMinValue(60);
		m_FrequencyEnvelope.SetMaxValue(140);

		m_Oscillator.SetFrequency(80);
	}

	void Beat(void) override
	{
		m_Envelope.Trigger();
		m_FrequencyEnvelope.Trigger();
		m_Oscillator.Reset();
	}
	
	T Process(void) override
	{
		m_Oscillator.SetFrequency(m_FrequencyEnvelope.Process());

		return m_Envelope.Process() * m_Oscillator.Process();
	}

private:
	AttackDecaySustainReleaseEnvelopeFilter<T, SampleRate> m_Envelope;
	AttackDecaySustainReleaseEnvelopeFilter<T, SampleRate> m_FrequencyEnvelope;
	OscillatorFilter<T, SampleRate> m_Oscillator;
};

#endif