#pragma once
#ifndef CLIPPER_FILTER_H
#define CLIPPER_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"

template<typename T, uint32_t SampleRate>
class ClipperFilter : public Filter<T, SampleRate>
{
private:
	static constexpr float MaxBias = 0.5f;
	static constexpr float MaxWarmth = 2.0f;
	static constexpr float ProbeStep = 0.001f;
	static constexpr float MinSlope = 0.2f;

public:
	ClipperFilter(void)
		: m_Asymmetry(0),
		m_PositiveThreshold(1),
		m_NegativeThreshold(-1),
		m_EvenHarmonics(0),
		m_Warmth(0),
		m_Bias(0),
		m_BiasOffset(0),
		m_BiasSlopeInverse(1),
		m_WarmthCoefficient(0)
	{
	}

	virtual void SetAsymmetry(cc_ratio_t Value)
	{
		m_Asymmetry = Value;
	}
	float GetAsymmetry(void) const
	{
		return m_Asymmetry;
	}

	//[0, 2]
	virtual void SetPositiveThreshold(cc_float<0, 2> Value)
	{
		m_PositiveThreshold = Value;

		UpdateBias();
	}
	float GetPositiveThreshold(void) const
	{
		return m_PositiveThreshold;
	}

	virtual void SetNegativeThreshold(cc_float<-2, 0> Value)
	{
		m_NegativeThreshold = Value;

		UpdateBias();
	}
	float GetNegativeThreshold(void) const
	{
		return m_NegativeThreshold;
	}

	void SetEvenHarmonics(cc_ratio_t Value)
	{
		m_EvenHarmonics = Value;

		UpdateBias();
	}
	float GetEvenHarmonics(void) const
	{
		return m_EvenHarmonics;
	}

	void SetWarmth(cc_ratio_t Value)
	{
		m_Warmth = Value;

		m_WarmthCoefficient = Value * MaxWarmth;

		UpdateBias();
	}
	float GetWarmth(void) const
	{
		return m_Warmth;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			const T x = Buffer[i] + m_Bias;

			// warmth: soft pre-compression, keeps the sign and is monotonic
			const T warm = x / (1.0f + m_WarmthCoefficient * Math::Absolute(x));

			Buffer[i] = (Shape(warm) - m_BiasOffset) * m_BiasSlopeInverse;
		}
	}

	FILTER_FORWARD_DECLARATION()

protected:
	virtual T Shape(T Value) = 0;

	void UpdateBias(void)
	{
		m_Bias = m_EvenHarmonics * MaxBias * m_PositiveThreshold;

		const T warm = m_Bias / (1.0f + m_WarmthCoefficient * Math::Absolute(m_Bias));

		m_BiasOffset = Shape(warm);

		const T warmAbove = (m_Bias + ProbeStep) / (1.0f + m_WarmthCoefficient * Math::Absolute(m_Bias + ProbeStep));
		const T warmBelow = (m_Bias - ProbeStep) / (1.0f + m_WarmthCoefficient * Math::Absolute(m_Bias - ProbeStep));

		float slope = (Shape(warmAbove) - Shape(warmBelow)) / (2 * ProbeStep);
		if (slope < MinSlope)
			slope = MinSlope;

		m_BiasSlopeInverse = 1.0f / slope;
	}

private:
	float m_Asymmetry;
	T m_PositiveThreshold;
	T m_NegativeThreshold;

	float m_EvenHarmonics;
	float m_Warmth;
	float m_Bias;
	T m_BiasOffset;
	float m_BiasSlopeInverse;
	float m_WarmthCoefficient;
};

#endif