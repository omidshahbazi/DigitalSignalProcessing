#pragma once
#ifndef REVERB_H
#define REVERB_H

#include "IDSP.h"
#include "../Filters/BufferFilter.h"

template <typename T, uint32 SampleRate, uint16 MaxDelayTime>
class Reverb : public IDSP<T, SampleRate>
{
public:
	Reverb(void)
	{
		SetDelayTime(0.5);
		SetFeedback(-6.02);
	}

	//[0, MaxDelayTime]
	void SetDelayTime(float Value)
	{
		m_Buffer.SetTime(Value);
	}
	float GetDelayTime(void) const
	{
		return m_Buffer.GetTime();
	}

	//[-20dB, 0dB]
	void SetFeedback(float Value)
	{
		ASSERT(-20 <= Value && Value <= 0, "Invalid Value %f", Value);

		m_Buffer.SetFeedback(Value);
	}
	float GetFeedback(void) const
	{
		return m_Buffer.GetFeedback();
	}

	//[0, 1]
	void SetWetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Buffer.SetOutputMixRate(Value);
	}
	float GetWetRate(void) const
	{
		return m_Buffer.GetOutputMixRate();
	}

	void Reset(void)
	{
		m_Buffer.Reset();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = m_Buffer.Record(Buffer[i]);
	}

private:
	BufferFilter<T, SampleRate, MaxDelayTime> m_Buffer;
};

#endif
