#pragma once
#ifndef PHASER_H
#define PHASER_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/OscillatorFilter.h"
#include "../Filters/DelayFilter.h"

template <typename T>
class Phaser : public IDSP<T>
{
private:
	static const uint8 DELAY_STAGE_COUNT = 2;

public:
	Phaser(uint32 SampleRate)
		: m_Oscillator(SampleRate),
		  m_Delays{{SampleRate, MAX_DELAY_TIME * 2}, {SampleRate, MAX_DELAY_TIME * 2}},
		  m_Depth(0),
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

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T output = Buffer[i];

			T modulationIndex = abs(m_Oscillator.Process()) * m_Depth;

			for (uint8 j = 0; j < DELAY_STAGE_COUNT; ++j)
			{
				m_Delays[j].Process(output);

				output += m_Delays[j].GetLerpedSample(modulationIndex, modulationIndex - (int32)modulationIndex);
			}

			output /= DELAY_STAGE_COUNT;

			Buffer[i] = Math::Lerp(Buffer[i], output, m_WetRate);
		}
	}

private:
	OscillatorFilter<T> m_Oscillator;
	DelayFilter<T> m_Delays[DELAY_STAGE_COUNT];
	float m_Depth;
	float m_WetRate;

public:
	static constexpr float MAX_DELAY_TIME = 0.025;
	static constexpr float MAX_DEPTH = 100;
};

#endif
