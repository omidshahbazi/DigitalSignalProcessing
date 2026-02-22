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
	void SetThreshold(float Value)
	{
		m_NoiseGateFilter.SetThreshold(Value);
	}
	float GetThreshold(void) const
	{
		return m_NoiseGateFilter.GetThreshold();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = m_NoiseGateFilter.Process(Buffer[i]);
	}

private:
	NoiseGateFilter<T, SampleRate> m_NoiseGateFilter;
};

#endif