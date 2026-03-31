#pragma once
#ifndef ATTACK_DECAY_ENVELOPE_FILTER_H
#define ATTACK_DECAY_ENVELOPE_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class AttackDecayEnvelopeFilter : public Filter<T, SampleRate>
{
public:
	enum class Segments
	{
		Idle = 0,
		Attack,
		Decay,

		COUNT
	};

public:
	AttackDecayEnvelopeFilter(void)
		: m_CurrSegment(Segments::Idle),
		  m_SegmentTime{},
		  m_MinValue(0),
		  m_MaxValue(1),
		  m_Curve(0),
		  m_RawValue(0.0f),
		  m_TargetValue(0.0f),
		  m_Coeff(0.0f)
	{
		m_SegmentTime[(uint8)Segments::Idle] = 0.05;

		SetAttackTime(50 ms);
		SetDecayTime(50 ms);
	}

	//(0, 1s]
	void SetAttackTime(float Value)
	{
		ASSERT(0 < Value && Value <= 1, "Invalid Value %f", Value);

		m_SegmentTime[(uint8)Segments::Attack] = Value;
		if (m_CurrSegment == Segments::Attack)
			PrepareSegment();
	}
	float GetAttackTime(void) const
	{
		return m_SegmentTime[(uint8)Segments::Attack];
	}

	//(0, 1s]
	void SetDecayTime(float Value)
	{
		ASSERT(0 < Value && Value <= 1, "Invalid Value %f", Value);

		m_SegmentTime[(uint8)Segments::Decay] = Value;
		if (m_CurrSegment == Segments::Decay)
			PrepareSegment();
	}
	float GetDecayTime(void) const
	{
		return m_SegmentTime[(uint8)Segments::Decay];
	}

	// [0, 100]
	// 0: Full Linear Curve
	// Positive: Log Curve
	void SetCurve(float Value)
	{
		ASSERT(0 <= Value, "Invalid Value %f", Value);

		m_Curve = Value;
		PrepareSegment();
	}
	float GetCurve(void) const
	{
		return m_Curve;
	}

	//[-Infinity, +Infinity]
	void SetMinValue(float Value)
	{
		m_MinValue = Value;
	}
	float GetMinValue(void) const
	{
		return m_MinValue;
	}

	//[-Infinity, +Infinity]
	void SetMaxValue(float Value)
	{
		m_MaxValue = Value;
	}
	float GetMaxValue(void) const
	{
		return m_MaxValue;
	}

	float GetValue(void) const
	{
		return Math::Lerp(m_MinValue, m_MaxValue, m_RawValue);
	}

	void Trigger(void)
	{
		m_CurrSegment = Segments::Attack;
		PrepareSegment();
	}

	Segments GetCurrentSegment(void) const
	{
		return m_CurrSegment;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Process();
	}

	T Process(void)
	{
		if (m_CurrSegment == Segments::Idle)
			return (T)m_MinValue;

		if (m_Curve <= 0.01f) // Linear Mode
		{
			m_RawValue += m_Coeff;
		}
		else // Exponential Mode (Fast & Professional)
		{
			// V = Target + (Current - Target) * Coeff
			m_RawValue = m_TargetValue + (m_RawValue - m_TargetValue) * m_Coeff;
		}

		m_RawValue = Math::Clamp01(m_RawValue);

		bool segmentFinished = false;
		if (m_CurrSegment == Segments::Attack && m_RawValue >= 0.999f)
		{
			m_RawValue = 1.0f;
			segmentFinished = true;
		}
		else if (m_CurrSegment == Segments::Decay && m_RawValue <= 0.001f)
		{
			m_RawValue = 0.0f;
			segmentFinished = true;
		}

		if (segmentFinished)
		{
			// حرکت به سگمنت بعدی با استفاده از Moderate خودت
			m_CurrSegment = (Segments)Math::Moderate((uint8)m_CurrSegment + 1, (uint8)Segments::COUNT);
			PrepareSegment();
		}

		return (T)GetValue();
	}

private:
	void PrepareSegment(void)
	{
		float timeInSeconds = m_SegmentTime[(uint8)m_CurrSegment];
		uint32 sampleCount = static_cast<uint32>(timeInSeconds * SampleRate);

		if (m_CurrSegment == Segments::Idle || sampleCount == 0)
		{
			m_Coeff = 0;
			m_TargetValue = 0;
			return;
		}

		if (m_Curve <= 0.01f) // Linear
		{
			m_TargetValue = (m_CurrSegment == Segments::Attack) ? 1.0f : 0.0f;
			m_Coeff = (m_TargetValue - m_RawValue) / sampleCount;
		}
		else // Exponential
		{
			m_TargetValue = (m_CurrSegment == Segments::Attack) ? 1.1f : -0.1f;
			m_Coeff = std::exp(-m_Curve / (timeInSeconds * SampleRate));
		}
	}

private:
	Segments m_CurrSegment;
	float m_SegmentTime[(uint8)Segments::COUNT];
	float m_MinValue;
	float m_MaxValue;
	float m_Curve;
	float m_RawValue;
	float m_TargetValue;
	float m_Coeff;
};

#endif