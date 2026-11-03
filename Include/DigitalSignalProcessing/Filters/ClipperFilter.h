#pragma once
#ifndef CLIPPER_FILTER_H
#define CLIPPER_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"

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
		m_PositiveThreshold(0),
		m_NegativeThreshold(0),
		m_EvenHarmonics(0),
		m_Warmth(0),
		m_Bias(0),
		m_BiasOffset(0),
		m_BiasSlopeInverse(1),
		m_WarmthCoefficient(0)
	{
	}

	virtual void SetInputGain(LinearGain Value)
	{}

	//[0, 1]
	virtual void SetAsymmetry(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Asymmetry = Value;
	}
	float GetAsymmetry(void) const
	{
		return m_Asymmetry;
	}

	//[0, 2]
	virtual void SetPositiveThreshold(float Value)
	{
		ASSERT(0 <= Value && Value <= 2, "Invalid Value %f", Value);

		m_PositiveThreshold = Value;

		UpdateBias();
	}
	float GetPositiveThreshold(void) const
	{
		return m_PositiveThreshold;
	}

	//[-2, 0]
	virtual void SetNegativeThreshold(float Value)
	{
		ASSERT(-2 <= Value && Value <= 0, "Invalid Value %f", Value);

		m_NegativeThreshold = Value;
	}
	float GetNegativeThreshold(void) const
	{
		return m_NegativeThreshold;
	}

	//[0, 1] Amount of even harmonics (0 = none)
	void SetEvenHarmonics(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_EvenHarmonics = Value;

		UpdateBias();
	}
	float GetEvenHarmonics(void) const
	{
		return m_EvenHarmonics;
	}

	//[0, 1] Softness of the knee (0 = unchanged, 1 = warm and compressed)
	void SetWarmth(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

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