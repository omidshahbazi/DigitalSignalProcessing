#pragma once
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "IDSP.h"
#include "../Debug.h"
#include "../Math.h"
#include "../Filters/EnvelopeFollowerFilter.h"

// TODO: Algorithm seems incorrect
template <typename T>
class Compressor : public IDSP<T>
{
public:
	Compressor(uint32 SampleRate)
		: m_EnvelopeFollowerFilter(SampleRate)
	{
		SetRatio(1);
	}

	//(0, 1]
	void SetRatio(float Value)
	{
		ASSERT(0 < Value && Value <= 2, "Invalid Value");

		m_Ratio = Value;

		printf("rat %f\n", Value);
	}
	float GetRatio(void) const
	{
		return m_Ratio;
	}

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T envelope = m_EnvelopeFollowerFilter.Process(Buffer[i]);
			T absValue = fabs(Buffer[i]);

			Buffer[i] = (m_Threshold + (absValue - (m_Threshold / m_Ratio))) * Math::Sign(Buffer[i]);
		}
	}

private:
	EnvelopeFollowerFilter<T> m_EnvelopeFollowerFilter;
	T m_Threshold;
	float m_Ratio;
};

#endif
