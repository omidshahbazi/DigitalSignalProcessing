#pragma once
#ifndef OSCILATOR_FILTER_H
#define OSCILATOR_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Debug.h"
#include "../ContextCallback.h"

template <typename T, uint32 SampleRate>
class OscillatorFilter : public Filter<T, SampleRate>
{
public:
	typedef ContextCallback<T, T> OscillatorFunction;

public:
	OscillatorFilter(void)
		: m_Frequency(0),
		  m_PhaseOffset(0),
		  m_DeltaPhase(0),
		  m_Phase(0)
	{
		SetSineWaveFunction();
	}

	void SetFunction(OscillatorFunction Function)
	{
		ASSERT(Function != nullptr, "Invalid Function");

		m_Function = Function;
	}

	void SetSineWaveFunction(void)
	{
		SetFunction({this, [](void *Context, T Value)
					 { return (T)Math::Sin(Value * Math::TWO_PI_VALUE); }});
	}

	void SetTriangleWaveFunction(void)
	{
		SetFunction({this, [](void *Context, T Value)
					 {
						 T t = (2 * Value) - 1;
						 return 2 * Math::Absolute(t) - 1;
					 }});
	}

	void SetSawtoothWaveFunction(void)
	{
		SetFunction({this, [](void *Context, T Value)
					 { return -1 * ((Value * 2) - 1); }});
	}

	void SetRampWaveFunction(void)
	{
		SetFunction({this, [](void *Context, T Value)
					 { return (Value * 2) - 1; }});
	}

	void SetSquareWaveFunction(void)
	{
		SetFunction({this, [](void *Context, T Value)
					 { return (T)(Value < 0.5 ? 1 : -1); }});
	}

	//(0, MAX_FREQUENCY]
	void SetFrequency(Frequency Value)
	{
		ASSERT(0 < Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_Frequency = Value;

		m_DeltaPhase = m_Frequency / SampleRate;
	}
	Frequency GetFrequency(void) const
	{
		return m_Frequency;
	}

	//[0, 1)
	void SetPhaseOffset(float Value)
	{
		ASSERT(0 < Value && Value < 1, "Invalid Value %f", Value);

		m_PhaseOffset = Value;
	}
	float GetPhaseOffset(void) const
	{
		return m_PhaseOffset;
	}

	void Reset(void)
	{
		m_Phase = 0;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Process();
	}

	T Process(void)
	{
		T value = m_Function(Math::Moderate(m_Phase + m_PhaseOffset, 1));

		m_Phase = Math::Moderate(m_Phase + m_DeltaPhase, 1);

		return value;
	}

private:
	Frequency m_Frequency;
	float m_PhaseOffset;
	OscillatorFunction m_Function;

	float m_DeltaPhase;
	float m_Phase;
};

#endif