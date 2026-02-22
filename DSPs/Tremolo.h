#pragma once
#ifndef TREMOLO_H
#define TREMOLO_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/OscillatorFilter.h"

template <typename T, uint32 SampleRate>
class Tremolo : public IDSP<T, SampleRate>
{
public:
	Tremolo(void)
		: m_Depth(0),
		  m_WetRate(0)
	{
		SetDepth(0.5);
		SetRate(10);
	}

	//[0, 1]
	void SetDepth(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0Hz, 25Hz]
	void SetRate(float Value)
	{
		ASSERT(0 < Value && Value <= 25, "Invalid Value %f", Value);

		m_Oscillator.SetFrequency(Value);
	}
	float GetRate(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	//[0, 1]
	void SetWetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_WetRate = Value;
	}
	float GetWetRate(void) const
	{
		return m_WetRate;
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T modulation = (1 - m_Depth) + (m_Depth * 0.5 * (1 + m_Oscillator.Process()));

			Buffer[i] = Mix(Buffer[i], Buffer[i] * modulation);
		}
	}
	
protected:
	T Mix(T A, T B) override
	{
		return Math::CrossFadeMix(A, B, m_WetRate);	
	}

private:
	OscillatorFilter<T, SampleRate> m_Oscillator;
	float m_Depth;
	float m_WetRate;
};

#endif
