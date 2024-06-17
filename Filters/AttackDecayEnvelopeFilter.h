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
		  m_RawValue(0.00001),
		  m_TriggerValue(0),
		  m_Curve(0),
		  m_CurveValue(0)
	{
		m_SegmentTime[(uint8)Segments::Idle] = 0.05f;
		SetAttackTime(0.05);
		SetDecayTime(0.05);
	}

	//[0, 1]
	void SetAttackTime(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_SegmentTime[(uint8)Segments::Attack] = Value;
	}
	float GetAttackTime(void) const
	{
		return m_SegmentTime[(uint8)Segments::Attack];
	}

	//[0, 1]
	void SetDecayTime(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_SegmentTime[(uint8)Segments::Decay] = Value;
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
		ASSERT(0 <= Value, "Invalid Value");

		m_Curve = Value;
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
		return (m_RawValue * (m_MaxValue - m_MinValue)) + m_MinValue;
	}

	void Trigger(void)
	{
		m_CurrSegment = Segments::Attack;
		m_TriggerValue = m_RawValue;
		m_CurveValue = 0;
	}

	Segments GetCurrentSegment(void) const
	{
		return m_CurrSegment;
	}

	T Process(T Value) override
	{
		return Process();
	}

	T Process(void)
	{
		float beginValue;
		float endValue;

		uint32 sampleCount = (uint32)(m_SegmentTime[(uint8)m_CurrSegment] * SampleRate);

		switch (m_CurrSegment)
		{
		case Segments::Idle:
			beginValue = 0;
			endValue = 0;
			break;

		case Segments::Attack:
			beginValue = m_TriggerValue;
			endValue = 1;
			break;

		case Segments::Decay:
			beginValue = 1;
			endValue = 0;
			break;

		default:
			ASSERT(false, "Unhandled Type");
		}

		float step = 0;

		if (m_Curve == 0)
			step = (endValue - beginValue) / sampleCount;
		else
			step = (endValue - beginValue) / (1 - Math::Exponential(m_Curve));

		if (step >= 0)
			step = Math::Max(step, Math::EPSILON);
		else
			step = Math::Min(step, -Math::EPSILON);

		float prevRawValue = m_RawValue;

		if (m_Curve == 0)
			m_RawValue += step;
		else
		{
			m_CurveValue += (m_Curve / sampleCount);

			m_RawValue = beginValue + (step * (1 - Math::Exponential(m_CurveValue)));
			if (Math::IsNAN(m_RawValue))
				m_RawValue = 0;
		}

		if (m_CurrSegment != Segments::Idle)
		{
			if ((m_CurrSegment == Segments::Attack && prevRawValue >= 1) ||
				(m_CurrSegment == Segments::Decay && prevRawValue <= 0))
			{
				m_CurrSegment = (Segments)Math::Wrap((uint8)m_CurrSegment + 1, (uint8)Segments::Idle, (uint8)Segments::COUNT - 1);

				m_CurveValue = 0;
			}
		}

		if (m_CurrSegment == Segments::Idle)
			m_RawValue = 0;

		return GetValue();
	}

private:
	Segments m_CurrSegment;
	float m_SegmentTime[(uint8)Segments::COUNT];
	float m_MinValue;
	float m_MaxValue;
	float m_RawValue;
	float m_TriggerValue;
	float m_Curve;
	float m_CurveValue;
};

#endif