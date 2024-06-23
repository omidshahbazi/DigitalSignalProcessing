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
	enum class Types
	{
		Floor = 0,
		Middle,
		High
	};

public:
	Tom(void)
		: m_Type(Types::Floor)
	{
		SetType(Types::Floor);
	}

	void SetType(Types Value)
	{
		m_Type = Value;

		switch (m_Type)
		{
		case Types::Floor:
			m_Envelope.SetAttackTime(0.02);
			m_Envelope.SetDecayTime(0.4);
			m_Envelope.SetMinValue(0);
			m_Envelope.SetMaxValue(1);

			m_Oscillator.SetFrequency(170);
			m_Oscillator1.SetFrequency(80);
			break;

		case Types::Middle:
			break;

		case Types::High:
			break;

		default:
			ASSERT(false, "Unhandled Type");
		}
	}
	Types GetType(void) const
	{
		return m_Type;
	}

	void Beat(void) override
	{
		m_Envelope.Trigger();
		m_Oscillator.Reset();
		m_Oscillator1.Reset();
	}

	T Process(void) override
	{
		return Math::Lerp(m_Noise.Process(), m_Oscillator1.Process() + m_Oscillator.Process(), 0.955) * m_Envelope.Process();
	}

private:
	Types m_Type;
	AttackDecayEnvelopeFilter<T, SampleRate> m_Envelope;
	WhiteNoiseFilter<T, SampleRate> m_Noise;
	OscillatorFilter<T, SampleRate> m_Oscillator;
	OscillatorFilter<T, SampleRate> m_Oscillator1;
};

#endif