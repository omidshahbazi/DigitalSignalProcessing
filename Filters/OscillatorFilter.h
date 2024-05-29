#pragma once
#ifndef OSCILATOR_FILTER_H
#define OSCILATOR_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"
#include "../ContextCallback.h"

template <typename T, uint32 SampleRate>
class OscillatorFilter : public Filter<T>
{
public:
	typedef ContextCallback<T, T> OscillatorFunction;

public:
	OscillatorFilter(void)
		: m_Freuency(0),
		  m_PhaseOffset(0),
		  m_DeltaPhase(0),
		  m_Phase(0)
	{
		SetFunction({this, [](void *Context, T Value)
					 { return Math::Sin(Value); }});
	}

	void SetFunction(OscillatorFunction Function)
	{
		ASSERT(Function != nullptr, "Invalid Function");

		m_Function = Function;
	}

	//(0, MAX_FREQUENCY]
	void SetFrequency(float Value)
	{
		ASSERT(0 < Value && Value <= MAX_FREQUENCY, "Invalid Value");

		m_Freuency = Value;

		m_DeltaPhase = Math::TWO_PI_VALUE * m_Freuency / SampleRate;
	}
	float GetFrequency(void) const
	{
		return m_Freuency;
	}

	void SetPhaseOffset(float Value)
	{
		m_PhaseOffset = Value;
	}
	float GetPhaseOffset(void) const
	{
		return m_PhaseOffset;
	}

	T Process(T Value) override
	{
		return Process();
	}

	T Process(void)
	{
		T value = m_Function(Math::Moderate(m_Phase + m_PhaseOffset, Math::TWO_PI_VALUE));

		m_Phase = Math::Moderate(m_Phase + m_DeltaPhase, Math::TWO_PI_VALUE);

		return value;
	}

	static T DefaultFunction(T Value)
	{
		return;
	}

private:
	float m_Freuency;
	float m_PhaseOffset;
	OscillatorFunction m_Function;

	float m_DeltaPhase;
	float m_Phase;
};

#endif