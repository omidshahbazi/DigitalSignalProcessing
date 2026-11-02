#pragma once
#ifndef DIODE_CLIPPER_FILTER_H
#define DIODE_CLIPPER_FILTER_H

#include "ClipperFilter.h"

enum class DiodeClipperFilterTypes : uint8_t
{
	Silicon = 0,
	Germanium,
	LED
};

template<typename T, uint32_t SampleRate, DiodeClipperFilterTypes Type>
class DiodeClipperFilter : public ClipperFilter<T, SampleRate>
{
private:
	typedef ClipperFilter<T, SampleRate> Base;

public:
	//[0, 1]
	void SetAsymmetry(float Value) override
	{
		Base::SetAsymmetry(Value);

		float threshold = 0;
		if constexpr (Type == DiodeClipperFilterTypes::Silicon)
			threshold = 0.65;
		else if constexpr (Type == DiodeClipperFilterTypes::Germanium)
			threshold = 0.35;
		else if constexpr (Type == DiodeClipperFilterTypes::LED)
			threshold = 1.5;

		Base::SetPositiveThreshold(threshold);
		Base::SetNegativeThreshold(-(threshold - (Base::GetAsymmetry() * (threshold * 0.5))));
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			if (Buffer[i] >= 0)
				Buffer[i] = Math::TanH(Buffer[i] / Base::GetPositiveThreshold()) * Base::GetPositiveThreshold();
			else
				Buffer[i] = Math::TanH(Buffer[i] / -Base::GetNegativeThreshold()) * -Base::GetNegativeThreshold();
		}
	}

	FILTER_FORWARD_DECLARATION()
};

template<typename T, uint32_t SampleRate>
using SiliconDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Silicon>;

template<typename T, uint32_t SampleRate>
using GermaniumDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Germanium>;

template<typename T, uint32_t SampleRate>
using LEDDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::LED>;

#endif