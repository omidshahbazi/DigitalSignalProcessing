#pragma once
#ifndef CHORUS_H
#define CHORUS_H

#include "IDSP.h"
#include "../Math.h"
#include "../Filters/OscillatorFilter.h"
#include "../Filters/DelayFilter.h"

template <typename T, uint32 SampleRate>
class Chorus : public IDSP<T, SampleRate>
{
public:
	static constexpr float MAX_DELAY_TIME = 0.025;
	static constexpr float MAX_DEPTH = 100;

public:
	Chorus(void)
		: m_Depth(0),
		  m_WetRate(0)
	{
		SetDepth(1);
		SetRate(1);
		SetWetRate(0.5);
	}

	//[0, MAX_DEPTH]
	void SetDepth(float Value)
	{
		ASSERT(0 <= Value && Value <= MAX_DEPTH, "Invalid Value");

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0.01, 4]
	void SetRate(float Value)
	{
		ASSERT(0 < Value && Value <= 4, "Invalid Value");

		m_Oscillator.SetFrequency(Value);
	}
	float GetRate(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	//[0, 1]
	void SetWetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

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
			m_Delay.Process(Buffer[i]);

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * m_Depth;

			T delayedSample = m_Delay.GetLerpedSample(modulationIndex, Math::Fraction(modulationIndex));

			Buffer[i] = Math::Lerp(Buffer[i], delayedSample, m_WetRate);
		}
	}

private:
	OscillatorFilter<T, SampleRate> m_Oscillator;
	DelayFilter<T, SampleRate, MAX_DELAY_TIME * 2> m_Delay;
	float m_Depth;
	float m_WetRate;
};

#endif
