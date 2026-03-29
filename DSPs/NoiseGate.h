#pragma once
#ifndef NOISE_GATE_H
#define NOISE_GATE_H

#include "IDSP.h"
#include "../Filters/NoiseGateFilter.h"

template <typename T, uint32 SampleRate>
class NoiseGate : public IDSP<T, SampleRate>
{
public:
	//[-90dB, 0dB]
	void SetThreshold(dBGain Value)
	{
		m_NoiseGateFilter.SetThreshold(Value);
	}
	dBGain GetThreshold(void) const
	{
		return m_NoiseGateFilter.GetThreshold();
	}

	void Process(T *Buffer, uint8 Count) override
	{
		m_NoiseGateFilter.Process(Buffer[i], Count);
	}

private:
	NoiseGateFilter<T, SampleRate> m_NoiseGateFilter;
};

#endif