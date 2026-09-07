#pragma once
#ifndef WHITE_NOISE_FILTER_H
#define WHITE_NOISE_FILTER_H

#include "Filter.h"
#include "../Debug.h"

template <typename T, uint32_t SampleRate>
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
	void SetSeed(int32_t Value)
	{
		ASSERT(Value != 0, "Value must be non-zero");

		m_Seed = Value;
	}
	int32_t GetSeed(void) const
	{
		return m_Seed;
	}

	//void Process(T *Buffer, uint8_t Count) override
	//{
	//	uint32_t seed = m_Seed;

	//	for (uint8_t i = 0; i < Count; ++i)
	//	{
	//		seed ^= seed << 13;
	//		seed ^= seed >> 17;
	//		seed ^= seed << 5;

	//		Buffer[i] = seed * MULTIPLIER;
	//	}

	//	m_Seed = seed;
	//}

	void Process(T* Buffer, uint8_t Count) override
	{
		uint32_t seed = (uint32_t)m_Seed;

		for (uint8_t i = 0; i < Count; ++i)
		{
			seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
			float u1 = (float)(seed) / 4294967296.0f;
			if (u1 < 1e-7f) 
				u1 = 1e-7f;

			seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
			float u2 = (float)(seed) / 4294967296.0f;

			Buffer[i] = (Math::SquareRoot(-2.0f * Math::Log(u1)) * Math::Cos(Math::TWO_PI_VALUE * u2)) / 3.0f;
		}

		m_Seed = (int32_t)seed;
	}

	FILTER_FORWARD_DECLARATION()

private:
	int32_t m_Seed;
};

#endif