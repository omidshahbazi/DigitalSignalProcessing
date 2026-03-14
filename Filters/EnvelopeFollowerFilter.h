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
		  m_AttackSlope(0),
		  m_ReleaseSlope(0),
		  m_Envelope(0.1)
	{
		SetAttackTime(1 ms);
		SetReleaseTime(10 ms);
	}

	//[100ns, 500ms]
	void SetAttackTime(float Value)
	{
		ASSERT(100 ns <= Value && Value <= 500 ms, "Invalid Value %f", Value);

		m_AttackTime = Value;

		m_AttackSlope = Math::Exponential(-(1.0F / SampleRate) / m_AttackTime);
	}
	float GetAttackTime(void) const
	{
		return m_AttackTime;
	}

	//[5ms, 5s]
	void SetReleaseTime(float Value)
	{
		ASSERT(0.005 <= Value && Value <= 5, "Invalid Value %f", Value);

		m_ReleaseTime = Value;
		
		m_ReleaseSlope = Math::Exponential(-(1.0F / SampleRate) / m_ReleaseTime);
	}
	float GetReleaseTime(void) const
	{
		return m_ReleaseTime;
	}

	T Process(T Value) override
	{
		Value = Math::Absolute(Value);

		float currentSlope = ((m_Envelope > Value) ? m_ReleaseSlope : m_AttackSlope);

		m_Envelope = Math::Lerp(Value, m_Envelope, currentSlope);

		return m_Envelope;
	}

protected:
	float m_AttackTime;
	float m_ReleaseTime;

	double m_RatioMultiplier;
	double m_AttackSlope;
	double m_ReleaseSlope;
	double m_Envelope;
};

#endif