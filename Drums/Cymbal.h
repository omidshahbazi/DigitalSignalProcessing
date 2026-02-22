#pragma once
#ifndef CYMBAL_H
#define CYMBAL_H

#include "DrumsPart.h"
#include "../Filters/AttackDecaySustainReleaseEnvelopeFilter.h"
#include "../Filters/WhiteNoiseFilter.h"
#include "../Filters/BandPassFilter.h"

template <typename T, uint32 SampleRate>
class Cymbal : public DrumsPart<T, SampleRate>
{
public:
	enum class Types
	{
		HiHat = 0,
		Ride,
		Crash
	};

public:
	Cymbal(void)
		: m_Type(Types::HiHat),
		  m_MaxCutoffFrequency(0)
	{
		SetType(Types::HiHat);
	}

	void SetType(Types Value)
	{
		m_Type = Value;

		m_Envelope.SetMinValue(0);
		m_Envelope.SetMaxValue(1);

		m_FrequencyEnvelope.SetAttackTime(0.005);
		m_FrequencyEnvelope.SetDecayTime(0.1);
		m_FrequencyEnvelope.SetSustainTime(0.0001);
		m_FrequencyEnvelope.SetReleaseTime(0.3);

		m_BandPass.SetQualityFactory(1);

		switch (m_Type)
		{
		case Types::HiHat:
			m_Envelope.SetAttackTime(0.005);
			m_Envelope.SetDecayTime(0.1);
			m_Envelope.SetSustainTime(0.0001);
			m_Envelope.SetReleaseTime(0);

			m_MaxCutoffFrequency = 8 KHz;

			m_FrequencyEnvelope.SetMinValue(m_MaxCutoffFrequency);
			m_FrequencyEnvelope.SetMaxValue(6 KHz);
			break;

		case Types::Ride:
			m_Envelope.SetAttackTime(0.005);
			m_Envelope.SetDecayTime(0.1);
			m_Envelope.SetSustainTime(0.0001);
			m_Envelope.SetReleaseTime(0.8);

			m_MaxCutoffFrequency = 10 KHz;

			m_FrequencyEnvelope.SetMinValue(m_MaxCutoffFrequency);
			m_FrequencyEnvelope.SetMaxValue(8 KHz);

			break;

		case Types::Crash:
			m_Envelope.SetAttackTime(0.005);
			m_Envelope.SetDecayTime(0.1);
			m_Envelope.SetSustainTime(0.0001);
			m_Envelope.SetReleaseTime(0.5);

			m_MaxCutoffFrequency = 12 KHz;

			m_FrequencyEnvelope.SetMinValue(m_MaxCutoffFrequency);
			m_FrequencyEnvelope.SetMaxValue(10 KHz);

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
		m_FrequencyEnvelope.Trigger();
	}

	T Process(void) override
	{
		m_BandPass.SetFrequencies(m_FrequencyEnvelope.Process(), m_MaxCutoffFrequency);

		T sample = m_BandPass.Process(m_Noise.Process()) * m_Envelope.Process();

		return sample;
	}

private:
	Types m_Type;
	AttackDecaySustainReleaseEnvelopeFilter<T, SampleRate> m_Envelope;
	AttackDecaySustainReleaseEnvelopeFilter<T, SampleRate> m_FrequencyEnvelope;
	WhiteNoiseFilter<T, SampleRate> m_Noise;
	float m_MaxCutoffFrequency;
	BandPassFilter<T, SampleRate> m_BandPass;
};

#endif