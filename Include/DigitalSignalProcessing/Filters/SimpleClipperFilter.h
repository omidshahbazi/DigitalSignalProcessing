#pragma once
#ifndef SIMPLE_CLIPPER_FILTER_H
#define SIMPLE_CLIPPER_FILTER_H

#include "ClipperFilter.h"

enum class SimpleClipperFilterTypes : uint8_t
{
	Rational = 0,
	Polynomial,
	Hyperbolic,
	Hard
};

template<typename T, uint32_t SampleRate, SimpleClipperFilterTypes Type>
class SimpleClipperFilter : public ClipperFilter<T, SampleRate>
{
private:
	typedef ClipperFilter<T, SampleRate> Base;

public:
	SimpleClipperFilter(void)
	{
		Base::SetAsymmetry(0);
		Base::SetPositiveThreshold(1);
		Base::SetNegativeThreshold(-1);
	}

	FILTER_FORWARD_DECLARATION()

protected:
	T Shape(T Value) override
	{
		const float NegativeScale = 1.0f - 0.5f * Base::GetAsymmetry();

		if constexpr (Type == SimpleClipperFilterTypes::Rational)
		{
			const float PositiveThresh = Base::GetPositiveThreshold() - (0.2f * Base::GetAsymmetry());
			const float NegativeThresh = Base::GetNegativeThreshold() + (0.3f * Base::GetAsymmetry());

			if (Value > PositiveThresh)
			{
				const T excess = Value - PositiveThresh;
				return PositiveThresh + (excess / (1.0f + excess * 1.5f));
			}
			if (Value < NegativeThresh)
			{
				const T excess = Value - NegativeThresh;
				return NegativeThresh + (excess / (1.0f - excess * 1.5f));
			}
			return Value;
		}
		else if constexpr (Type == SimpleClipperFilterTypes::Polynomial)
			return (Value >= 0) ? Math::TanH(Value) : NegativeScale * Math::TanH(Value / NegativeScale);
		else if constexpr (Type == SimpleClipperFilterTypes::Hyperbolic)
			return (Value >= 0) ? Math::ATan(Value * Math::HALF_PI_VALUE) / Math::HALF_PI_VALUE : NegativeScale * Math::ATan(Value * Math::HALF_PI_VALUE / NegativeScale) / Math::HALF_PI_VALUE;
		else
		{
			const float NegativeLimit = Base::GetNegativeThreshold() * NegativeScale;

			return Math::Clamp(Value, NegativeLimit, Base::GetPositiveThreshold());
		}
	}
};

template<typename T, uint32_t SampleRate>
using RationalSimpleClipperFilter = SimpleClipperFilter<T, SampleRate, SimpleClipperFilterTypes::Rational>;

template<typename T, uint32_t SampleRate>
using PolynomialSimpleClipperFilter = SimpleClipperFilter<T, SampleRate, SimpleClipperFilterTypes::Polynomial>;

template<typename T, uint32_t SampleRate>
using HyperbolicSimpleClipperFilter = SimpleClipperFilter<T, SampleRate, SimpleClipperFilterTypes::Hyperbolic>;

template<typename T, uint32_t SampleRate>
using HardSimpleClipperFilter = SimpleClipperFilter<T, SampleRate, SimpleClipperFilterTypes::Hard>;

#endif