#pragma once
#ifndef DELAY_FILTER_H
#define DELAY_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Memory.h"

template <typename T>
class DelayFilter : public Filter<T>
{
public:
	DelayFilter(uint32 SampleRate, float MaxTime)
		: m_SampleRate(SampleRate),
		  m_MaxTime(MaxTime),
		  m_Time(0),
		  m_Feedback(0),
		  m_OutputMixRate(0),
		  m_Buffer(nullptr),
		  m_BufferLength(0),
		  m_BufferIndex(0)
	{
		ASSERT(MIN_SAMPLE_RATE <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate");
		ASSERT(MaxTime * SampleRate * sizeof(T) <= SAMPLE_RATE_22050 * sizeof(T), "The result of MaxTime * SampleRate * sizeof(T) must not exceed from %i", SAMPLE_RATE_22050 * sizeof(T));

		m_Buffer = Memory::Allocate<T>(m_MaxTime * m_SampleRate, true);

		SetTime(m_MaxTime);
		SetFeedback(1);
		SetOutputMixRate(0.5);
	}

	~DelayFilter(void)
	{
		Memory::Deallocate(m_Buffer);
	}

	float GetMaxTime(void) const
	{
		return m_MaxTime;
	}

	//[0, MaxTime]
	void SetTime(float Value)
	{
		ASSERT(0 <= Value && Value <= m_MaxTime, "Invalid Value");

		m_Time = Value;

		m_BufferLength = Math::Max(m_Time * m_SampleRate, 1);
	}
	float GetTime(void) const
	{
		return m_Time;
	}

	//[0, 1]
	void SetFeedback(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Feedback = Value;
	}
	float GetFeedback(void) const
	{
		return m_Feedback;
	}

	//[0, 1]
	void SetOutputMixRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_OutputMixRate = Value;
	}
	float GetOutputMixRate(void) const
	{
		return m_OutputMixRate;
	}

	uint32 GetBufferLength(void) const
	{
		return m_BufferLength;
	}

	T GetSample(int32 Offset = 0) const
	{
		return GetCircularSample(m_BufferIndex + Offset);
	}

	T GetLerpedSample(int32 Offset, float Fraction) const
	{
		int32 index = m_BufferIndex + Offset;

		return Math::Lerp(GetCircularSample(index), GetCircularSample(index + 1), Fraction);
	}

	void MoveForward(void)
	{
		m_BufferIndex = (m_BufferIndex + 1) % m_BufferLength;
	}

	T Process(T Value, bool Additive)
	{
		T delayedSample = GetCircularSample(m_BufferIndex);

		m_Buffer[m_BufferIndex] = Value;

		if (Additive)
			m_Buffer[m_BufferIndex] += delayedSample;

		MoveForward();

		return Math::Lerp(Value, delayedSample, m_OutputMixRate);
	}

private:
	T GetCircularSample(int32 Index) const
	{
		return m_Buffer[Index % m_BufferLength] * m_Feedback;
	}

	T Process(T Value) override
	{
		return 0;
	}

private:
	uint32 m_SampleRate;
	float m_MaxTime;
	float m_Time;
	float m_Feedback;
	float m_OutputMixRate;

	T *m_Buffer;
	uint32 m_BufferLength;
	uint32 m_BufferIndex;
};

#endif