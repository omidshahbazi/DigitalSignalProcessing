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

	T Process(T Value) override
	{
		return Process();
	}

	T Process(void)
	{
		m_Seed *= 16807;

		return (m_Seed * MULTIPLIER);
	}

private:
	int32 m_Seed;
};

#endif