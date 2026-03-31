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
		  m_Curve(0),
		  m_RawValue(0.0f),
		  m_TargetValue(0.0f),
		  m_Multiplier(0.0f),
		  m_AutoReleaseCounter(0)
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
		PrepareSegment();
	}

	void Release(void)
	{
		m_CurrSegment = Segments::Release;
		PrepareSegment();
	}

	Segments GetCurrentSegment(void) const { return m_CurrSegment; }

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Process();
	}

	T Process(void)
	{
		if (m_CurrSegment == Segments::Idle)
			return m_MinValue;

		if (m_Curve <= 0.001f) // Linear
		{
			m_RawValue += m_Multiplier;
		}
		else // Exponential
		{
			m_RawValue = m_TargetValue + (m_RawValue - m_TargetValue) * m_Multiplier;
		}

		bool next = false;
		switch (m_CurrSegment)
		{
		case Segments::Attack:
			if (m_RawValue >= 1.0f)
			{
				m_RawValue = 1.0f;
				next = true;
			}
			break;
		case Segments::Decay:
			if (m_RawValue <= m_SustainLevel)
			{
				m_RawValue = m_SustainLevel;
				next = true;
			}
			break;
		case Segments::Sustain:
			if (m_SegmentTime[(uint8)Segments::Sustain] > 0)
			{
				if (++m_AutoReleaseCounter >= m_AutoReleaseLimit)
					next = true;
			}
			break;
		case Segments::Release:
			if (m_RawValue <= 0.0001f)
			{
				m_RawValue = 0.0f;
				next = true;
			}
			break;
		default:
			break;
		}

		if (next)
		{
			uint8 nextS = (uint8)m_CurrSegment + 1;
			m_CurrSegment = (nextS >= (uint8)Segments::COUNT) ? Segments::Idle : (Segments)nextS;
			PrepareSegment();
		}

		return GetValue();
	}

private:
	void PrepareSegment(void)
	{
		float time = m_SegmentTime[(uint8)m_CurrSegment];
		uint32 samples = static_cast<uint32>(time * SampleRate);
		m_AutoReleaseCounter = 0;

		if (m_CurrSegment == Segments::Idle || (samples == 0 && m_CurrSegment != Segments::Sustain))
		{
			// Bypass logic: jump to next segment if time is 0
			if (m_CurrSegment != Segments::Idle)
			{
				if (m_CurrSegment == Segments::Attack)
					m_RawValue = 1.0f;
				else if (m_CurrSegment == Segments::Decay)
					m_RawValue = m_SustainLevel;
				else if (m_CurrSegment == Segments::Release)
					m_RawValue = 0.0f;

				m_CurrSegment = (Segments)((uint8)m_CurrSegment + 1);
				PrepareSegment();
			}
			return;
		}

		if (m_CurrSegment == Segments::Sustain)
		{
			m_TargetValue = m_SustainLevel;
			m_Multiplier = (m_Curve <= 0.001f) ? 0.0f : 1.0f;
			m_AutoReleaseLimit = samples;
			return;
		}

		if (m_Curve <= 0.001f) // Linear Pre-calculation
		{
			m_TargetValue = (m_CurrSegment == Segments::Attack) ? 1.0f : (m_CurrSegment == Segments::Decay) ? m_SustainLevel
																											: 0.0f;
			m_Multiplier = (m_TargetValue - m_RawValue) / samples;
		}
		else // Exponential Pre-calculation using Math::Exponential
		{
			m_TargetValue = (m_CurrSegment == Segments::Attack) ? 1.1f : (m_CurrSegment == Segments::Decay) ? m_SustainLevel - 0.1f
																											: -0.1f;

			float exponent = -m_Curve / (float)samples;
			m_Multiplier = Math::Exponential(exponent);
		}
	}

private:
	Segments m_CurrSegment;
	float m_SegmentTime[(uint8)Segments::COUNT];
	float m_SustainLevel;
	T m_MinValue;
	T m_MaxValue;
	float m_Curve;
	float m_RawValue;
	float m_TargetValue;
	float m_Multiplier;
	uint32 m_AutoReleaseCounter;
	uint32 m_AutoReleaseLimit;
};

#endif