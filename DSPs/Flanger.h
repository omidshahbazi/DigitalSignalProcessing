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
		SetTime(5 ms);
		SetFeedback(-6.02);
		SetDepth(0.001);
		SetRate(1);
	}

	//(0ms, 3ms]
	void SetDepth(float Value)
	{
		ASSERT(0 < Value && Value <= 3 ms, "Invalid Value %f", Value);

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	//(0Hz, 2Hz]
	void SetRate(float Value)
	{
		ASSERT(0 < Value && Value <= 4, "Invalid Value %f", Value);

		m_Oscillator.SetFrequency(Value);
	}
	float GetRate(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	//[SILENCE_GAIN, NORMAL_GAIN]
	void SetFeedback(dBGain Value)
	{
		ASSERT(SILENCE_GAIN <= Value && Value <= 0, "Invalid Value %f", Value);

		m_Buffer.SetFeedback(Value);
	}
	dBGain GetFeedback(void) const
	{
		return m_Buffer.GetFeedback();
	}

	void Clear(void)
	{
		m_Buffer.Clear();
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			m_Buffer.Record(Buffer[i]);

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * (m_Depth * SampleRate);

			T delayedSample = m_Buffer.GetLerpedSample(modulationIndex, Math::Fraction(modulationIndex));

			Buffer[i] = Math::SoftClip(Mix(Buffer[i], delayedSample));
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
