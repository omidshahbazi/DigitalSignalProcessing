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
	DiodeClipperFilter(void)
	{
		SetAsymmetry(0);
	}

	//[0, 1]
	void SetAsymmetry(float Value) override
	{
		Base::SetAsymmetry(Value);

		float threshold = 0;
		if constexpr (Type == DiodeClipperFilterTypes::Silicon)
			threshold = 0.65f;
		else if constexpr (Type == DiodeClipperFilterTypes::Germanium)
			threshold = 0.35f;
		else if constexpr (Type == DiodeClipperFilterTypes::LED)
			threshold = 1.5f;

		Base::SetPositiveThreshold(threshold);
		Base::SetNegativeThreshold(-(threshold - (Base::GetAsymmetry() * (threshold * 0.5f))));
	}

	FILTER_FORWARD_DECLARATION()

protected:
	T Shape(T Value) override
	{
		if (Value >= 0)
			return Math::TanH(Value / Base::GetPositiveThreshold()) * Base::GetPositiveThreshold();

		return Math::TanH(Value / -Base::GetNegativeThreshold()) * -Base::GetNegativeThreshold();
	}
};

template<typename T, uint32_t SampleRate>
using SiliconDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Silicon>;

template<typename T, uint32_t SampleRate>
using GermaniumDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Germanium>;

template<typename T, uint32_t SampleRate>
using LEDDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::LED>;

#endif