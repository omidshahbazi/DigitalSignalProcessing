#pragma once
#ifndef FLANGER_H
#define FLANGER_H

#include "IDSP.h"
#include "../Math.h"
#include "../Filters/OscillatorFilter.h"
#include "../Filters/BufferFilter.h"

template <typename T, uint32 SampleRate>
class Flanger : public IDSP<T, SampleRate>
{
public:
	Flanger(void)
		: m_Depth(0)
	{
		SetTime(0.005);
		SetFeedback(-6.02);
		SetDepth(1);
		SetRate(1);
	}

	//[0, 100]
	void SetDepth(float Value)
	{
		ASSERT(0 <= Value && Value <= 100, "Invalid Value %f", Value);

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0, 0.5Hz]
	void SetRate(float Value)
	{
		ASSERT(0 < Value && Value <= 4, "Invalid Value %f", Value);

		m_Oscillator.SetFrequency(Value);
	}
	float GetRate(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	//[SILENCE_GAIN_dB, 0dB]
	void SetFeedback(float Value)
	{
		ASSERT(SILENCE_GAIN_dB <= Value && Value <= 0, "Invalid Value %f", Value);

		m_Buffer.SetFeedback(Value);
	}
	float GetFeedback(void) const
	{
		return m_Buffer.GetFeedback();
	}

	void Reset(void)
	{
		m_Buffer.Reset();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			m_Buffer.Record(Buffer[i]);

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * m_Depth;

			T delayedSample = m_Buffer.GetLerpedSample(modulationIndex, Math::Fraction(modulationIndex));

			Buffer[i] = Mix(Buffer[i], delayedSample);
		}
	}

protected:
	void SetTime(float Value)
	{
		m_Buffer.SetTime(Value);
	}
	
	T Mix(T A, T B) override
	{
		return Math::AdditiveMix(A, B);
	}

private:
	OscillatorFilter<T, SampleRate> m_Oscillator;
	BufferFilter<T, SampleRate, 1> m_Buffer;
	float m_Depth;
};

#endif
