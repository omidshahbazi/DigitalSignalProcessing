#pragma once
#ifndef FLANGER_H
#define FLANGER_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Filters/OscillatorFilter.h"
#include "DigitalSignalProcessing/Filters/BufferFilter.h"

template <typename T, uint32_t SampleRate>
class Flanger : public IDSP<T, SampleRate>
{
public:
	Flanger(void)
		: m_Depth(0)
	{
		SetTime(5 ms);
		SetFeedback(dBGain(-6.02));
		SetDepth(0.001);
		SetRate(Frequency(1));
	}
	
	//(0ms, 3ms]
	void SetDepth(float Value)
	{
		ASSERT_VALUE_RANGE_MAX_INCLUSIVE(Value, 0, 3 ms);

		m_Depth = Value;
	}
	float GetDepth(void) const
	{
		return m_Depth;
	}

	void SetRate(cc_Frequency<0, 3> Value)
	{
		m_Oscillator.SetFrequency(Value);
	}
	Frequency GetRate(void) const
	{
		return m_Oscillator.GetFrequency();
	}

	void SetFeedback(cc_dBGain<SILENCE_GAIN_VALUE, NORMAL_GAIN_VALUE> Value)
	{
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

	void Process(T *Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			m_Buffer.Record(Buffer[i]);

			T modulationIndex = Math::Absolute(m_Oscillator.Process()) * (m_Depth * SampleRate);

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
