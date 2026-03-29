#pragma once
#ifndef WHITE_NOISE_FILTER_H
#define WHITE_NOISE_FILTER_H

#include "Filter.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class WhiteNoiseFilter : public Filter<T, SampleRate>
{
private:
	static constexpr float MULTIPLIER = 4.6566129e-010f;

public:
	WhiteNoiseFilter(void)
		: m_Seed(0)
	{
		SetSeed(1);
	}

	//[-Infinity, (0), +Infinity]
	void SetSeed(int32 Value)
	{
		ASSERT(Value != 0, "Value must be non-zero");

		m_Seed = Value;
	}
	int32 GetSeed(void) const
	{
		return m_Seed;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			m_Seed *= 16807;

			Buffer[i] = m_Seed * MULTIPLIER;
		}
	}

private:
	int32 m_Seed;
};

#endif