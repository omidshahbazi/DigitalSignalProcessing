#pragma once
#ifndef DRUM_MATCHINE_H
#define DRUM_MATCHINE_H

#include "IDSP.h"
#include "../Filters/AttackDecayEnvelopeFilter.h"
#include "../Filters/WhiteNoiseFilter.h"

template <typename T, uint32 SampleRate>
class DrumMachine : public IDSP<T, SampleRate>
{
public:
	DrumMachine(void)
	{
		m_SnareEnvelope.SetAttackTime(0.01);
		m_SnareEnvelope.SetDecayTime(0.2);
		m_SnareEnvelope.SetMaxValue(1);
		m_SnareEnvelope.SetMinValue(0);

		m_SnareEnvelope.Trigger();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			T snr_env_out = m_SnareEnvelope.Process();

			// Get the next snare sample
			T noise_out = m_WhiteNoiseFilter.Process();
			// Set the sample to the correct volume
			noise_out *= snr_env_out;

			Buffer[i] = noise_out;
		}
	}

private:
	AttackDecayEnvelopeFilter<T, SampleRate> m_SnareEnvelope;
	WhiteNoiseFilter<T, SampleRate> m_WhiteNoiseFilter;
};

#endif