#pragma once
#ifndef KICK_H
#define KICK_H

#include "DrumsPart.h"
#include "../Filters/AttackDecayEnvelopeFilter.h"
#include "../Filters/OscillatorFilter.h"

template <typename T, uint32 SampleRate>
class Kick : public DrumsPart<T, SampleRate>
{
public:
	Kick(void)
	{
		m_PitchEnvelope.SetAttackTime(0.01);
		m_PitchEnvelope.SetDecayTime(0.05);
		m_PitchEnvelope.SetMaxValue(400);
		m_PitchEnvelope.SetMinValue(50);

		m_VolumeEnvelope.SetAttackTime(0.01);
		m_VolumeEnvelope.SetDecayTime(1);
		m_VolumeEnvelope.SetMaxValue(1);
		m_VolumeEnvelope.SetMinValue(0);

		m_Oscillator.SetTriangleWaveFunction();
	}

	void Beat(void) override
	{
		m_PitchEnvelope.Trigger();
		m_VolumeEnvelope.Trigger();
	}

	T Process(void) override
	{
		m_Oscillator.SetFrequency(m_PitchEnvelope.Process());

		return m_Oscillator.Process() * m_VolumeEnvelope.Process();
	}

private:
	AttackDecayEnvelopeFilter<T, SampleRate> m_PitchEnvelope;
	AttackDecayEnvelopeFilter<T, SampleRate> m_VolumeEnvelope;
	OscillatorFilter<T, SampleRate> m_Oscillator;
};

#endif