#pragma once
#ifndef FREQUENCY_DETECTOR_H
#define FREQUENCY_DETECTOR_H

#include "FastFourierTransformer.h"
#include "Math.h"

template <typename T, uint32 SampleRate, uint16 SampleCount>
class FrequencyDetector
{
	static_assert(Math::IsPowerOfTwo(SampleCount), "SampleCount must be power-of-two");

public:
	FrequencyDetector(void)
		: m_Buffer{},
		  m_BufferIndex(0)
	{
	}

	void Process(const T *const Buffer, uint8 Count)
	{
		for (uint8 i = 0; i < Count; ++i)
			Process(Buffer[i]);
	}

	void Process(T Value)
	{
		m_Buffer[m_BufferIndex] = Value;

		m_BufferIndex = (m_BufferIndex + 1) & (SampleCount - 1);
	}

	Frequency CalculateFrequency(void) const
	{
		return FastFourierTransformer::CalculateFrequencyRaw<T, SampleRate, SampleCount>(m_Buffer, m_BufferIndex);
	}

	void Reset(void)
	{
		Memory::Set(m_Buffer, 0, SampleCount);

		m_BufferIndex = 0;
	}

private:
	T m_Buffer[SampleCount];
	uint16 m_BufferIndex;
};

#endif