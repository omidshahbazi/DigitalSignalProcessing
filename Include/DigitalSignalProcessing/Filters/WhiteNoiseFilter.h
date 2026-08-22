#pragma once
#ifndef WHITE_NOISE_FILTER_H
#define WHITE_NOISE_FILTER_H

#include "Filter.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class WhiteNoiseFilter : public Filter<T, SampleRate>
{
private:
	static constexpr float MULTIPLIER = 1.0 / 2147483648;

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

	//void Process(T *Buffer, uint8 Count) override
	//{
	//	uint32 seed = m_Seed;

	//	for (uint8 i = 0; i < Count; ++i)
	//	{
	//		seed ^= seed << 13;
	//		seed ^= seed >> 17;
	//		seed ^= seed << 5;

	//		Buffer[i] = seed * MULTIPLIER;
	//	}

	//	m_Seed = seed;
	//}

	void Process(T* Buffer, uint8 Count) override
	{
		uint32 seed = (uint32)m_Seed;

		for (uint8 i = 0; i < Count; ++i)
		{
			seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
			float u1 = (float)(seed) / 4294967296.0f;
			if (u1 < 1e-7f) 
				u1 = 1e-7f;

			seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
			float u2 = (float)(seed) / 4294967296.0f;

			Buffer[i] = (Math::SquareRoot(-2.0f * Math::Log(u1)) * Math::Cos(Math::TWO_PI_VALUE * u2)) / 3.0f;
		}

		m_Seed = (int32)seed;
	}

	FILTER_FORWARD_DECLARATION()

private:
	int32 m_Seed;
};

#endif