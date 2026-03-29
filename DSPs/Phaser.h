#pragma once
#ifndef PHASER_H
#define PHASER_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/OscillatorFilter.h"
#include "../Filters/BufferFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount = 2>
class Phaser : public IDSP<T, SampleRate>
{
public:
	Phaser(void)
		: m_Depth(0),
		  m_WetRate(0)
	{
		for (uint8 i = 0; i < StageCount; ++i)
		{
			m_Buffers[i].SetFeedback(MIN_GAIN);
			m_Buffers[i].SetTime(50 ms);
			m_Buffers[i].SetOutputMixRate(1);
		}

		SetDepth(1 ms);
		SetRate(2.5);
		SetWetRate(0.5);
	}

	//(0ms, 3ms]
	void SetDepth(float Value)
	{
		ASSERT(0 < Value && Value <= 3 ms, "Invalid Value %f", Value);

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0, 5Hz]
	void SetRate(float Value)
	{
		ASSERT(0 < Value && Value <= 5, "Invalid Value %f", Value);

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

	void Clear(void)
	{
		for (uint8 j = 0; j < StageCount; ++j)
			m_Buffers[j].Clear();
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			T output = Buffer[i];

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * (m_Depth * SampleRate);

			for (uint8 j = 0; j < StageCount; ++j)
			{
				m_Buffers[j].Record(output);

				output = Math::LinearCrossFadeMix(output, m_Buffers[j].GetLerpedSample(modulationIndex, Math::Fraction(modulationIndex)), 0.5);
			}

			Buffer[i] = Mix(Buffer[i], output);
		}
	}

protected:
	T Mix(T A, T B) override
	{
		return Math::AdditiveMix(A, B, m_WetRate);
	}

private:
	OscillatorFilter<T, SampleRate> m_Oscillator;
	BufferFilter<T, SampleRate, 1> m_Buffers[StageCount];
	float m_Depth;
	float m_WetRate;
};

#endif
