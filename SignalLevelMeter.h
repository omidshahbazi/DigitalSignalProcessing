#pragma once
#ifndef SIGNAL_LEVEL_METER_H
#define SIGNAL_LEVEL_METER_H

#include "Math.h"
#include "Log.h"

template <typename T, uint16 SampleCount>
class SignalLevelMeter
{
public:
	SignalLevelMeter(void)
		: m_Buffer{},
		  m_BufferIndex(0)
	{
	}

	void Process(T Value)
	{
		m_Buffer[m_BufferIndex] = Value;
		m_BufferIndex = Math::Wrap(m_BufferIndex + 1, 0, SampleCount - 1);
	}

	LinearGain GetMean(void) const
	{
		double sum = 0;
		for (uint16 i = 0; i < SampleCount; ++i)
			sum += Math::Absolute(m_Buffer[i]);

		return sum / SampleCount;
	}

	LinearGain GetMin(void) const
	{
		T min = 1;
		for (uint16 i = 0; i < SampleCount; ++i)
			if (min > m_Buffer[i])
				min = m_Buffer[i];

		return min;
	}

	LinearGain GetMax(void) const
	{
		T max = -1;
		for (uint16 i = 0; i < SampleCount; ++i)
			if (max < m_Buffer[i])
				max = m_Buffer[i];

		return max;
	}

	LinearGain GetPeak(void) const
	{
		return Math::Max(GetMax(), Math::Absolute(GetMin()));
	}

	LinearGain GetRMS(void) const
	{
		double sum = 0;
		for (uint16 i = 0; i < SampleCount; ++i)
			sum += Math::Power(m_Buffer[i], 2);

		return Math::Max(0, Math::SquareRoot(sum / SampleCount));
	}

	T GetCrestFactor(void) const
	{
		return dBGain(GetPeak()) - dBGain(GetRMS());
	}

	void Reset(void)
	{
		Memory::Set(m_Buffer, 0, SampleCount);

		m_BufferIndex = 0;
	}

	void Print(void) const
	{
		Log::WriteInfo("Samples Min: %fdB Max: %fdB Mean: %fdB Peak: %fdB Crest Factor: %fdB", dBGain(GetMin()), dBGain(GetMax()), dBGain(GetMean()), dBGain(GetPeak()), GetCrestFactor());
	}

private:
	T m_Buffer[SampleCount];
	uint16 m_BufferIndex;
};
#endif