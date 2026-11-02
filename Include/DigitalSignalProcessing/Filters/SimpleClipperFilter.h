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
	void Process(T* Buffer, uint8_t Count) override
	{
		if constexpr (Type == SimpleClipperFilterTypes::Rational)
		{
			const float PositiveThresh = Base::GetPositiveThreshold() - (0.2f * Base::GetAsymmetry());
			const float NegativeThresh = Base::GetNegativeThreshold() + (0.3f * Base::GetAsymmetry());

			for (uint8_t i = 0; i < Count; ++i)
			{
				T x = Buffer[i];

				if (x > PositiveThresh)
				{
					T excess = x - PositiveThresh;
					Buffer[i] = PositiveThresh + (excess / (1 + excess * 1.5));
				}
				else if (x < NegativeThresh)
				{
					T excess = x - NegativeThresh;
					Buffer[i] = NegativeThresh + (excess / (1 - excess * 1.5));
				}
				else
					Buffer[i] = x;
			}
		}
		else if constexpr (Type == SimpleClipperFilterTypes::Polynomial)
		{
			const float NegativeScale = 1.0f - 0.5f * Base::GetAsymmetry();

			for (uint8_t i = 0; i < Count; ++i)
				Buffer[i] = (Buffer[i] >= 0) ? Math::TanH(Buffer[i]) : NegativeScale * Math::TanH(Buffer[i] / NegativeScale);
		}
		else if constexpr (Type == SimpleClipperFilterTypes::Hyperbolic)
		{
			const float NegativeScale = 1.0f - 0.5f * Base::GetAsymmetry();

			for (uint8_t i = 0; i < Count; ++i)
				Buffer[i] = (Buffer[i] >= 0) ? Math::ATan(Buffer[i] * Math::HALF_PI_VALUE) / Math::HALF_PI_VALUE : NegativeScale * Math::ATan(Buffer[i] * Math::HALF_PI_VALUE / NegativeScale) / Math::HALF_PI_VALUE;
		}
		else if constexpr (Type == SimpleClipperFilterTypes::Hard)
		{
			const float NegativeScale = 1.0f - 0.5f * Base::GetAsymmetry();
			const float PositiveThresh = Base::GetPositiveThreshold();
			const float NegativeThresh = Base::GetNegativeThreshold() * NegativeScale;

			for (uint8_t i = 0; i < Count; ++i)
				Buffer[i] = Math::Clamp(Buffer[i], NegativeThresh, PositiveThresh);
		}
	}

	FILTER_FORWARD_DECLARATION()
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