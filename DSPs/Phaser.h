#pragma once
#ifndef PHASER_H
#define PHASER_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/OscillatorFilter.h"
#include "../Filters/DelayFilter.h"

template <typename T, uint32 SampleRate>
class Phaser : public IDSP<T, SampleRate>
{
private:
	static constexpr uint8 DELAY_STAGE_COUNT = 2;

public:
	Phaser(void)
		: m_Depth(0),
		  m_WetRate(0)
	{
		for (uint8 i = 0; i < DELAY_STAGE_COUNT; ++i)
			m_Delays[i].SetTime(0.05);

		SetDepth(1);
		SetRate(1);
		SetWetRate(0.5);
	}

	//[0, 100]
	void SetDepth(float Value)
	{
		ASSERT(0 <= Value && Value <= 100, "Invalid Value");

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0.1, 3.7]
	void SetRate(float Value)
	{
		ASSERT(0.1 < Value && Value <= 25, "Invalid Value");

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

	void Reset(void)
	{
		for (uint8 j = 0; j < DELAY_STAGE_COUNT; ++j)
			m_Delays[j].Reset();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T output = Buffer[i];

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * m_Depth;

			for (uint8 j = 0; j < DELAY_STAGE_COUNT; ++j)
			{
				m_Delays[j].Process(output);

				output = (output + m_Delays[j].GetLerpedSample(modulationIndex, Math::Fraction(modulationIndex))) * 0.5;
			}

			Buffer[i] = Math::Lerp(Buffer[i], output, m_WetRate);
		}
	}

private:
	OscillatorFilter<T, SampleRate> m_Oscillator;
	DelayFilter<T, SampleRate, 1> m_Delays[DELAY_STAGE_COUNT];
	float m_Depth;
	float m_WetRate;
};

#endif
