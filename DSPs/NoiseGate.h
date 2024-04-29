#pragma once
#ifndef NOISE_GATE_H
#define NOISE_GATE_H

#include "IDSP.h"
#include "../Filters/NoiseGateFilter.h"

template <typename T>
class NoiseGate : public IDSP<T>
{
public:
	NoiseGate(uint32 SampleRate)
		: m_NoiseGateFilter(SampleRate)
	{
	}

	//[0dB, 80dB]
	void SetThreshold(float Value)
	{
		m_NoiseGateFilter.SetThreshold(Value);
	}
	float GetThreshold(void) const
	{
		return m_NoiseGateFilter.GetThreshold();
	}

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = m_NoiseGateFilter.Process(Buffer[i]);
	}

private:
	NoiseGateFilter<T> m_NoiseGateFilter;
};

#endif