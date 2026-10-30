#pragma once
#ifndef SIMPLE_CLIPPER_FILTER_H
#define SIMPLE_CLIPPER_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"

enum class SimpleClipperFilterTypes : uint8_t
{
	Soft = 0,
	Hard
};

template<typename T, uint32_t SampleRate, SimpleClipperFilterTypes Type>
class SimpleClipperFilter : public Filter<T, SampleRate>
{
public:
	SimpleClipperFilter(void) :
		m_Asymmetry(0)
	{
		SetAsymmetry(0);
	}

	//[0, 1]
	void SetParameter(float Value)
	{
		SetAsymmetry(Value);
	}
	float GetParameter(void) const
	{
		return GetAsymmetry();
	}

	//[0, 1]
	void SetAsymmetry(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Asymmetry = Value;
	}
	float GetAsymmetry(void) const
	{
		return m_Asymmetry;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		if constexpr (Type == SimpleClipperFilterTypes::Soft)
		{
			for (uint8_t i = 0; i < Count; ++i)
				Buffer[i] = Math::TanH(Buffer[i] + (m_Asymmetry * Buffer[i] * Buffer[i]));
		}
		else if constexpr (Type == SimpleClipperFilterTypes::Hard)
		{
			for (uint8_t i = 0; i < Count; ++i)
			{
				float x = Buffer[i];

				float positiveThresh = 0.7 - (0.2 * m_Asymmetry);
				float negativeThresh = -0.7 - (0.3 * m_Asymmetry);

				if (x > positiveThresh)
				{
					float excess = x - positiveThresh;
					Buffer[i] = positiveThresh + (excess / (1 + excess * 1.5));
				}
				else if (x < negativeThresh)
				{
					float excess = x - negativeThresh;
					Buffer[i] = negativeThresh + (excess / (1 - excess * 1.5));
				}
				else
					Buffer[i] = x;
			}
		}
	}

	FILTER_FORWARD_DECLARATION()

private:
	float m_Asymmetry;
};

template<typename T, uint32_t SampleRate>
using SoftClipperFilter = SimpleClipperFilter<T, SampleRate>;

template<typename T, uint32_t SampleRate>
using HardClipperFilter = SimpleClipperFilter<T, SampleRate>;

#endif