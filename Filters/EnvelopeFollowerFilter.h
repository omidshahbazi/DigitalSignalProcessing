#pragma once
#ifndef ENVELOPE_FOLLOWER_FILTER_H
#define ENVELOPE_FOLLOWER_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class EnvelopeFollowerFilter : public Filter<T, SampleRate>
{
public:
	EnvelopeFollowerFilter(void)
		: m_AttackTime(0),
		  m_ReleaseTime(0),
		  m_UseAbsoluteValue(false),
		  m_AttackSlope(0),
		  m_ReleaseSlope(0),
		  m_Envelope(0.1)
	{
		SetAttackTime(0.001);
		SetReleaseTime(0.001);
		SetUseAbsoluteValue(false);
	}

	//[0.001, 10]
	void SetAttackTime(float Value)
	{
		ASSERT(0.001 <= Value && Value <= 10, "Invalid Value %f", Value);

		m_AttackTime = Value;

		m_AttackSlope = Math::Exponential(-(1.0F / SampleRate) / m_AttackTime);
	}
	float GetAttackTime(void) const
	{
		return m_AttackTime;
	}

	//[0.001, 10]
	void SetReleaseTime(float Value)
	{
		ASSERT(0.001 <= Value && Value <= 10, "Invalid Value %f", Value);

		m_ReleaseTime = Value;

		m_ReleaseSlope = Math::Exponential(-(1.0F / SampleRate) / m_AttackTime);
	}
	float GetReleaseTime(void) const
	{
		return m_ReleaseTime;
	}

	void SetUseAbsoluteValue(bool Value)
	{
		m_UseAbsoluteValue = Value;
	}
	float GetUseAbsoluteValue(void) const
	{
		return m_UseAbsoluteValue;
	}

	T Process(T Value) override
	{
		Value = m_UseAbsoluteValue ? Math::Absolute(Value) : Value;

		float currentSlope = ((m_Envelope > Value) ? m_ReleaseSlope : m_AttackSlope);

		m_Envelope = Math::Lerp(Value, m_Envelope, currentSlope);

		return m_Envelope;
	}

protected:
	float m_AttackTime;
	float m_ReleaseTime;
	bool m_UseAbsoluteValue;

	double m_RatioMultiplier;
	double m_AttackSlope;
	double m_ReleaseSlope;
	double m_Envelope;
};

#endif