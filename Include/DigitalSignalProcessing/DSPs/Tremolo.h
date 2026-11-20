#pragma once
#ifndef TREMOLO_H
#define TREMOLO_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Filters/OscillatorFilter.h"

template <typename T, uint32_t SampleRate>
class Tremolo : public IDSP<T, SampleRate>
{
public:
	Tremolo(void)
		: m_Depth(0),
		m_WetRate(0)
	{
		SetDepth(0.5);
		SetRate(Frequency(10));
	}

	void SetDepth(cc_ratio_t Value)
	{
		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0Hz, 25Hz]
	void SetRate(oc_Frequency<0, 25> Value)
	{
		m_Oscillator.SetFrequency(Value);
	}
	Frequency GetRate(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	void SetWetRate(cc_ratio_t Value)
	{
		m_WetRate = Value;
	}
	float GetWetRate(void) const
	{
		return m_WetRate;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			T modulation = (1 - m_Depth) + (m_Depth * 0.5 * (1 + m_Oscillator.Process()));

			Buffer[i] = Mix(Buffer[i], Buffer[i] * modulation);
		}
	}

protected:
	T Mix(T A, T B) override
	{
		return Math::LinearCrossFadeMix(A, B, m_WetRate);
	}

private:
	OscillatorFilter<T, SampleRate> m_Oscillator;
	float m_Depth;
	float m_WetRate;
};

#endif
