#pragma once
#ifndef ATTACK_DECAY_SUSTAIN_RELEASE_ENVELOPE_FILTER_H
#define ATTACK_DECAY_SUSTAIN_RELEASE_ENVELOPE_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class AttackDecaySustainReleaseEnvelopeFilter : public Filter<T, SampleRate>
{
public:
	enum class Segments
	{
		Idle = 0,
		Attack,
		Decay,
		Sustain,
		Release,

		COUNT
	};

public:
	AttackDecaySustainReleaseEnvelopeFilter(void)
		: m_CurrSegment(Segments::Idle),
		  m_SegmentTime{},
		  m_SustainLevel(0),
		  m_MinValue(0),
		  m_MaxValue(1),
		  m_TriggerValue(0),
		  m_Curve(0),
		  m_CurveValue(0),
		  m_RawValue(0.00001),
		  m_ElapsedSustainTime(0)
	{
		SetAttackTime(50 ms);
		SetDecayTime(50 ms);
		SetSustainTime(50 ms);
		SetSustainLevel(500 ms);
		SetReleaseTime(50 ms);
	}

	//[0, 1s]
	// 0: Bypass
	void SetAttackTime(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_SegmentTime[(uint8)Segments::Attack] = Value;
	}
	float GetAttackTime(void) const
	{
		return m_SegmentTime[(uint8)Segments::Attack];
	}

	//[0, 1s]
	// 0: Bypass
	void SetDecayTime(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_SegmentTime[(uint8)Segments::Decay] = Value;
	}
	float GetDecayTime(void) const
	{
		return m_SegmentTime[(uint8)Segments::Decay];
	}

	//[0, 1s]
	// Positive: Auto Release Mode
	void SetSustainTime(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_SegmentTime[(uint8)Segments::Sustain] = Value;
	}
	float GetSustainTime(void) const
	{
		return m_SegmentTime[(uint8)Segments::Sustain];
	}

	//[0, 1]
	void SetSustainLevel(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_SustainLevel = Value;
	}
	float GetSustainLevel(void) const
	{
		return m_SustainLevel;
	}

	//[0, 1s]
	// 0: Bypass
	void SetReleaseTime(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_SegmentTime[(uint8)Segments::Release] = Value;
	}
	float GetReleaseTime(void) const
	{
		return m_SegmentTime[(uint8)Segments::Release];
	}

	// [0, 100]
	// 0: Full Linear Curve
	// Positive: Log Curve
	void SetCurve(float Value)
	{
		ASSERT(0 <= Value, "Invalid Value %f", Value);

		m_Curve = Value;
	}
	float GetCurve(void) const
	{
		return m_Curve;
	}

	//[-Infinity, +Infinity]
	void SetMinValue(T Value)
	{
		m_MinValue = Value;
	}
	T GetMinValue(void) const
	{
		return m_MinValue;
	}

	//[-Infinity, +Infinity]
	void SetMaxValue(T Value)
	{
		m_MaxValue = Value;
	}
	T GetMaxValue(void) const
	{
		return m_MaxValue;
	}

	T GetValue(void) const
	{
		return Math::Lerp(m_MinValue, m_MaxValue, m_RawValue);
	}

	void Trigger(void)
	{
		m_CurrSegment = Segments::Attack;
		m_TriggerValue = m_RawValue;
		m_CurveValue = 0;
		m_ElapsedSustainTime = 0;
	}

	void Release(void)
	{
		m_CurrSegment = Segments::Release;
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
		const bool bypassAttack = (GetAttackTime() == 0);
		const bool bypassDecay = (GetDecayTime() == 0);
		const bool isAutoReleaseMode = (GetSustainTime() != 0);
		const bool bypassRelease = (GetReleaseTime() == 0);

		float beginValue = 0;
		float endValue = 0;

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
			endValue = m_SustainLevel;
			break;

		case Segments::Sustain:
			beginValue = m_SustainLevel;
			endValue = m_SustainLevel;
			break;

		case Segments::Release:
			beginValue = m_SustainLevel;
			endValue = 0;
			break;

		default:
			ASSERT(false, "Unhandled Type");
		}

		float prevRawValue = m_RawValue;

		if (m_CurrSegment == Segments::Idle)
			m_RawValue = 0;
		else if (m_CurrSegment == Segments::Attack && bypassAttack)
			m_RawValue = m_SustainLevel;
		else if (m_CurrSegment == Segments::Decay && bypassDecay)
			m_RawValue = 1;
		else if (m_CurrSegment == Segments::Sustain && isAutoReleaseMode)
		{
			m_RawValue = m_SustainLevel;

			m_ElapsedSustainTime += 1.0 / SampleRate;
			if (GetSustainTime() <= m_ElapsedSustainTime)
				m_CurrSegment = Segments::Release;
		}
		else if (m_CurrSegment == Segments::Release && bypassRelease)
			m_RawValue = 0;
		else
		{
			uint32 sampleCount = (uint32)(m_SegmentTime[(uint8)m_CurrSegment] * SampleRate);

			float step = 0;
			if (m_Curve == 0)
				step = (endValue - beginValue) / sampleCount;
			else
				step = (endValue - beginValue) / (1 - Math::Exponential(m_Curve));

			if (step >= 0)
				step = Math::Max(step, Math::EPSILON);
			else
				step = Math::Min(step, -Math::EPSILON);

			if (m_Curve == 0)
				m_RawValue += step;
			else
			{
				m_CurveValue += (m_Curve / sampleCount);

				m_RawValue = beginValue + (step * (1 - Math::Exponential(m_CurveValue)));
				if (Math::IsNAN(m_RawValue))
					m_RawValue = 0;
			}

			m_RawValue = Math::Clamp01(m_RawValue);
		}

		if ((m_CurrSegment == Segments::Attack && prevRawValue >= 1) ||
			(m_CurrSegment == Segments::Decay && prevRawValue <= m_SustainLevel) ||
			(m_CurrSegment == Segments::Release && prevRawValue <= 0))
		{
			m_CurrSegment = (Segments)Math::Moderate((uint8)m_CurrSegment + 1, (uint8)Segments::COUNT);

			m_CurveValue = 0;
		}

		return GetValue();
	}

private:
	Segments m_CurrSegment;
	float m_SegmentTime[(uint8)Segments::COUNT];
	float m_SustainLevel;
	T m_MinValue;
	T m_MaxValue;
	T m_TriggerValue;
	float m_Curve;
	float m_CurveValue;
	T m_RawValue;
	float m_ElapsedSustainTime;
};

#endif
