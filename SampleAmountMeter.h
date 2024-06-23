#pragma once
#ifndef SAMPLE_AMOUNT_METER_H
#define SAMPLE_AMOUNT_METER_H

#include "Math.h"
#include "Log.h"

template <typename T, uint16 SampleCount>
class SampleAmountMeter
{
public:
	SampleAmountMeter(void)
		: m_Buffer{},
		  m_BufferIndex(0)
	{
	}

	void Record(T Value)
	{
		m_Buffer[m_BufferIndex] = Value;
		m_BufferIndex = Math::Wrap(m_BufferIndex + 1, 0, SampleCount - 1);
	}

	T GetAverage(void) const
	{
		double sum = 0;
		for (uint16 i = 0; i < SampleCount; ++i)
			sum += m_Buffer[i];

		return sum / SampleCount;
	}

	T GetMin(void) const
	{
		T min = 1;
		for (uint16 i = 0; i < SampleCount; ++i)
			if (min > m_Buffer[i])
				min = m_Buffer[i];

		return min;
	}

	T GetMax(void) const
	{
		T max = -1;
		for (uint16 i = 0; i < SampleCount; ++i)
			if (max < m_Buffer[i])
				max = m_Buffer[i];

		return max;
	}

	T GetAbsoluteMax(void) const
	{
		return Math::Max(GetMax(), Math::Absolute(GetMin()));
	}

	void Reset(void)
	{
		Memory::Set(m_Buffer, 0, SampleCount);

		m_BufferIndex = 0;
	}

	void Print(void) const
	{
		Log::WriteInfo("Samples Min: %f Max: %f Average: %f Absolute Max: %f", GetMin(), GetMax(), GetAverage(), GetAbsoluteMax());
	}

private:
	T m_Buffer[SampleCount];
	uint16 m_BufferIndex;
};
#endif