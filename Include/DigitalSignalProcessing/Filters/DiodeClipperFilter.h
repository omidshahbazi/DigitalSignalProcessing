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
		: m_KneeToThreshold(0)
	{
		if constexpr (Type == DiodeClipperFilterTypes::Silicon)
			SetKneeToThreshold(0.074);
		else if constexpr (Type == DiodeClipperFilterTypes::Germanium)
			SetKneeToThreshold(0.096);
		else if constexpr (Type == DiodeClipperFilterTypes::LED)
			SetKneeToThreshold(0.052);
		else
			NOT_IMPLEMENTED();

		SetAsymmetry(0);
	}

	void SetKneeToThreshold(oc_ratio_t Value)
	{
		m_KneeToThreshold = Value;
	}
	float GetKneeToThreshold(void) const
	{
		return m_KneeToThreshold;
	}

	void SetAsymmetry(cc_ratio_t Value) override
	{
		Base::SetAsymmetry(Value);

		float threshold = 0;
		if constexpr (Type == DiodeClipperFilterTypes::Silicon)
			threshold = 0.65f;
		else if constexpr (Type == DiodeClipperFilterTypes::Germanium)
			threshold = 0.35f;
		else if constexpr (Type == DiodeClipperFilterTypes::LED)
			threshold = 1.5f;
		else
			NOT_IMPLEMENTED();

		Base::SetNegativeThreshold(-(threshold - (Base::GetAsymmetry() * (threshold * 0.5f))));
		Base::SetPositiveThreshold(threshold);
	}

	FILTER_FORWARD_DECLARATION()

protected:
	T Shape(T Value) override
	{
		const float scale = (Value >= 0 ? Base::GetPositiveThreshold() : -Base::GetNegativeThreshold()) * m_KneeToThreshold;

		return Math::ASinH(Value / scale) * scale;
	}

private:
	float m_KneeToThreshold;
};

template<typename T, uint32_t SampleRate>
using SiliconDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Silicon>;

template<typename T, uint32_t SampleRate>
using GermaniumDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::Germanium>;

template<typename T, uint32_t SampleRate>
using LEDDiodeClipperFilter = DiodeClipperFilter<T, SampleRate, DiodeClipperFilterTypes::LED>;

#endif