#pragma once
#ifndef PHASER_H
#define PHASER_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Filters/OscillatorFilter.h"
#include "DigitalSignalProcessing/Filters/BufferFilter.h"

template <typename T, uint32_t SampleRate, uint8_t StageCount = 2>
class Phaser : public IDSP<T, SampleRate>
{
public:
	Phaser(void)
		: m_Depth(0),
		m_WetRate(0)
	{
		for (uint8_t i = 0; i < StageCount; ++i)
		{
			m_Buffers[i].SetFeedback(SILENCE_GAIN);
			m_Buffers[i].SetTime(50 ms);
			m_Buffers[i].SetOutputMixRate(1);
		}

		SetDepth(1 ms);
		SetRate(Frequency(2.5));
		SetWetRate(0.5);
	}

	//(0ms, 3ms]
	void SetDepth(float Value)
	{
		ASSERT_VALUE_RANGE_MAX_INCLUSIVE(Value, 0, 3 ms);

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	void SetRate(oc_Frequency<0, 5> Value)
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

	void Clear(void)
	{
		for (uint8_t j = 0; j < StageCount; ++j)
			m_Buffers[j].Clear();
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			T output = Buffer[i];

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * (m_Depth * SampleRate);

			for (uint8_t j = 0; j < StageCount; ++j)
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
