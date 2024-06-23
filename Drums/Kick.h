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
		m_Envelope.SetAttackTime(0.0001);
		m_Envelope.SetDecayTime(0.2);
		m_Envelope.SetSustainTime(0.2);
		m_Envelope.SetSustainLevel(1);
		m_Envelope.SetReleaseTime(0.4);
		m_Envelope.SetMinValue(0);
		m_Envelope.SetMaxValue(1);

		m_FrequencyEnvelope.SetAttackTime(0.0001);
		m_FrequencyEnvelope.SetDecayTime(0.02);
		m_FrequencyEnvelope.SetSustainTime(0.0001);
		m_FrequencyEnvelope.SetSustainLevel(1);
		m_FrequencyEnvelope.SetReleaseTime(0.02);
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