#pragma once
#ifndef TOM_H
#define TOM_H

#include "DrumsPart.h"
#include "../Filters/AttackDecayEnvelopeFilter.h"
#include "../Filters/WhiteNoiseFilter.h"
#include "../Filters/OscillatorFilter.h"

template <typename T, uint32 SampleRate>
class Tom : public DrumsPart<T, SampleRate>
{
public:
	Tom(void)
	{
		SetFrequency(170);

		m_Oscillator1.SetFrequency(80);

		m_Envelope.SetAttackTime(0.02);
		m_Envelope.SetDecayTime(0.4);
		m_Envelope.SetMaxValue(1);
		m_Envelope.SetMinValue(0);
	}

	//(0, MAX_FREQUENCY]
	void SetFrequency(float Value)
	{
		m_Oscillator.SetFrequency(Value);
	}

	float GetFrequency(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	void Beat(void) override
	{
		m_Envelope.Trigger();
	}

	T Process(void) override
	{
		return Math::Lerp(m_WhiteNoiseFilter.Process(), m_Oscillator1.Process() + m_Oscillator.Process(), 0.955) * m_Envelope.Process();
	}

private:
	AttackDecayEnvelopeFilter<T, SampleRate> m_Envelope;
	WhiteNoiseFilter<T, SampleRate> m_WhiteNoiseFilter;
	OscillatorFilter<T, SampleRate> m_Oscillator;
	OscillatorFilter<T, SampleRate> m_Oscillator1;
};

#endif