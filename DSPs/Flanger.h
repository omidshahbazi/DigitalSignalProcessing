#pragma once
#ifndef FLANGER_H
#define FLANGER_H

#include "IDSP.h"
#include "../Math.h"
#include "../Filters/OscillatorFilter.h"
#include "../Filters/DelayFilter.h"

template <typename T, uint32 SampleRate>
class Flanger : public IDSP<T, SampleRate>
{
public:
	Flanger(void)
		: m_Depth(0),
		  m_WetRate(0)
	{
		m_Delay.SetTime(0.05);

		SetDepth(1);
		SetRate(1);
		SetFeedback(0.5);
		SetWetRate(0.5);
	}

	//[0, 100]
	void SetDepth(float Value)
	{
		ASSERT(0 <= Value && Value <= 100, "Invalid Value %f", Value);

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0.01, 4]
	void SetRate(float Value)
	{
		ASSERT(0 < Value && Value <= 4, "Invalid Value %f", Value);

		m_Oscillator.SetFrequency(Value);
	}
	float GetRate(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	//[0, 1]
	void SetFeedback(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Delay.SetFeedback(Value);
	}
	float GetFeedback(void) const
	{
		return m_Delay.GetFeedback();
	}

	//[0, 1]
	void SetWetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_WetRate = Value;
	}
	float GetWetRate(void)
	{
		return m_WetRate;
	}

	void Reset(void)
	{
		m_Delay.Reset();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			m_Delay.Process(Buffer[i], true);

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * m_Depth;

			T delayedSample = m_Delay.GetLerpedSample(modulationIndex, Math::Fraction(modulationIndex));

			Buffer[i] = Math::Lerp(Buffer[i], delayedSample, m_WetRate);
		}
	}

private:
	OscillatorFilter<T, SampleRate> m_Oscillator;
	DelayFilter<T, SampleRate, 1> m_Delay;
	float m_Depth;
	float m_WetRate;
};

#endif
