#pragma once
#ifndef TRANSISTOR_CLIPPER_FILTER_H
#define TRANSISTOR_CLIPPER_FILTER_H

#include "ClipperFilter.h"

template<typename T, uint32_t SampleRate>
class TransistorClipperFilter : public ClipperFilter<T, SampleRate>
{
private:
	typedef ClipperFilter<T, SampleRate> Base;

private:
	static constexpr float PositiveCeiling = 0.7f;

public:
	TransistorClipperFilter(void)
		: m_PositiveInput(0),
		m_NegativeInput(0),
		m_NegativeOutput(0)
	{
		SetAsymmetry(0.3f);
	}

	//[0, 1]
	void SetAsymmetry(float Value) override
	{
		Base::SetAsymmetry(Value);

		const float positiveLimit = PositiveCeiling;
		const float negativeLimit = PositiveCeiling * (1.0f - Base::GetAsymmetry() * 0.5f);

		Base::SetPositiveThreshold(positiveLimit);
		Base::SetNegativeThreshold(-negativeLimit);

		//cutoff side: tanh(x / limit) * limit
		m_PositiveInput = 1.0f / positiveLimit;

		//saturation side: atan normalized so the slope at zero is 1 and the ceiling is negativeLimit
		m_NegativeInput = Math::HALF_PI_VALUE / negativeLimit;
		m_NegativeOutput = negativeLimit / Math::HALF_PI_VALUE;

		Base::UpdateBias();
	}

	FILTER_FORWARD_DECLARATION()

protected:
	T Shape(T Value) override
	{
		if (Value >= 0)
			return Math::TanH(Value * m_PositiveInput) * Base::GetPositiveThreshold();

		return Math::ATan(Value * m_NegativeInput) * m_NegativeOutput;
	}

private:
	float m_PositiveInput;
	float m_NegativeInput;
	float m_NegativeOutput;
};

#endif