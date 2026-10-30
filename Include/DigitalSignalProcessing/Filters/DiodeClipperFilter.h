#pragma once
#ifndef DIODE_CLIPPER_FILTER_H
#define DIODE_CLIPPER_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"

enum class DiodeClipperFilterTypes : uint8_t
{
	Silicon = 0,
	Germanium,
	LED
};

template<typename T, uint32_t SampleRate, DiodeClipperFilterTypes Type>
class DiodeClipperFilter : public Filter<T, SampleRate>
{
public:
	DiodeClipperFilter(void) :
		m_Asymmetry(0)
	{
		if constexpr (Type == DiodeClipperFilterTypes::Silicon)
		{
			m_PositiveThreshold = 0.85;
			m_NegativeThreshold = 0.45;
		}
		else if constexpr (Type == DiodeClipperFilterTypes::Germanium)
		{
			m_PositiveThreshold = 0.8;
			m_NegativeThreshold = 0.35;
		}
		else if constexpr (Type == DiodeClipperFilterTypes::LED)
		{
			m_PositiveThreshold = 1.5;
			m_NegativeThreshold = 0.8;
		}
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
		float asymmetry = m_Asymmetry * 0.5;

		for (uint8_t i = 0; i < Count; ++i)
		{
			if (Buffer[i] >= 0)
			{
				float scaled = (Buffer[i] / m_PositiveThreshold) * (1 - asymmetry);
				Buffer[i] = Math::TanH(scaled) * m_PositiveThreshold;
			}
			else
			{
				float scaled = (Buffer[i] / m_NegativeThreshold) * (1 + asymmetry);
				Buffer[i] = Math::TanH(scaled) * m_NegativeThreshold;
			}
		}
	}

	FILTER_FORWARD_DECLARATION()

private:
	float m_PositiveThreshold;
	float m_NegativeThreshold;
	float m_Asymmetry;
};

template<typename T, uint32_t SampleRate>
using SiliconDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Silicon>;

template<typename T, uint32_t SampleRate>
using GermaniumDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Germanium>;

template<typename T, uint32_t SampleRate>
using LEDDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::LED>;

#endif