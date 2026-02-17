#pragma once
#ifndef FREQUENCY_DETECTOR_H
#define FREQUENCY_DETECTOR_H

#include "FastFourierTransfomr.h"
#include "Memory.h"

// template <typename T, uint32 SampleRate, uint16 SampleCount>
// class FrequencyDetector
// {
// 	static_assert(Math::IsPowerOfTwo(SampleCount), "SampleCount must be power-of-two");

// public:
// 	FrequencyDetector(void)
// 		: m_Buffer{}
// 	{
// 	}

// 	void Record(T Value)
// 	{
// 		Memory::Copy(m_Buffer + 1, m_Buffer, SampleCount - 1);

// 		m_Buffer[SampleCount - 1] = Value;
// 	}

// 	float CalculateFrequency(void) const
// 	{
// 		return FastFourierTransfomr::CalculateFrequency<T, SampleRate, SampleCount>(m_Buffer);
// 	}

// private:
// 	T m_Buffer[SampleCount];
// };

template <typename T, uint32 SampleRate, uint16 SampleCount>
class FrequencyDetector
{
	static_assert(Math::IsPowerOfTwo(SampleCount), "SampleCount must be power-of-two");

public:
	FrequencyDetector(void)
		: m_Buffer{}
	{
	}

	void Process(T Value)
	{
		m_Buffer[m_BufferIndex] = Value;

		m_BufferIndex = Math::Wrap(m_BufferIndex + 1, 0, SampleCount - 1);
	}

	float CalculateFrequency(void) const
	{
		return FastFourierTransfomr::CalculateFrequency<T, SampleRate, SampleCount>(m_Buffer);
	}

private:
	T m_Buffer[SampleCount];
	uint16 m_BufferIndex;
};
#endif