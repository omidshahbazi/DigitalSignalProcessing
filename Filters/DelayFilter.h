#pragma once
#ifndef DELAY_FILTER_H
#define DELAY_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Memory.h"

template <typename T, uint32 SampleRate, uint16 MaxTime>
class DelayFilter : public Filter<T, SampleRate>
{
	static_assert(0 < MaxTime, "Invalid MaxTime");

public:
	DelayFilter(void)
		: m_Time(0),
		  m_Feedback(0),
		  m_OutputMixRate(0),
		  m_Reverse(false),
		  m_Buffer(nullptr),
		  m_TotalBufferLength(MaxTime * SampleRate),
		  m_BufferLength(0),
		  m_WriteBufferIndex(0),
		  m_ReadBufferIndex(0)
	{
		m_Buffer = Memory::Allocate<T>(m_TotalBufferLength, true);

		SetTime(MaxTime);
		SetFeedback(1);
		SetOutputMixRate(0.5);
	}

	~DelayFilter(void)
	{
		Memory::Deallocate(m_Buffer);
	}

	//[0, MaxTime]
	void SetTime(float Value)
	{
		ASSERT(0 <= Value && Value <= MaxTime, "Invalid Value");

		m_Time = Value;

		m_BufferLength = Math::Max(m_Time * SampleRate, 1);
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

	void SetReverse(bool Value)
	{
		m_Reverse = Value;
	}
	bool GetReverse(void) const
	{
		return m_Reverse;
	}

	uint32 GetBufferLength(void) const
	{
		return m_BufferLength;
	}

	T GetSample(uint32 Offset = 0) const
	{
		return GetCircularSample(m_ReadBufferIndex + Offset);
	}

	T GetLerpedSample(uint32 Offset, float Fraction) const
	{
		uint32 index = m_ReadBufferIndex + Offset;

		return Math::Lerp(GetCircularSample(index), GetCircularSample(index + 1), Fraction);
	}

	void MoveForward(void)
	{
		m_WriteBufferIndex = Math::Wrap(m_WriteBufferIndex + 1, 0, m_BufferLength - 1);
		m_ReadBufferIndex = Math::Wrap(m_ReadBufferIndex + (m_Reverse ? -1 : 1), 0, m_BufferLength - 1);
	}

	T Process(T Value) override
	{
		return Process(Value, false);
	}

	T Process(T Value, bool Additive)
	{
		T delayedSample = GetCircularSample(m_ReadBufferIndex);

		m_Buffer[m_WriteBufferIndex] = (Additive ? (Value + delayedSample) * 0.5F : Value);

		MoveForward();

		return Math::Lerp(Value, delayedSample, m_OutputMixRate);
	}

	void Reset(void)
	{
		Memory::Set(m_Buffer, 0, m_TotalBufferLength);

		m_WriteBufferIndex = 0;
		m_ReadBufferIndex = 0;
	}

private:
	T GetCircularSample(uint32 Index) const
	{
		return m_Buffer[Math::Wrap(Index, 0, m_BufferLength - 1)] * m_Feedback;
	}

private:
	float m_Time;
	float m_Feedback;
	float m_OutputMixRate;
	bool m_Reverse;

	T *m_Buffer;
	uint32 m_TotalBufferLength;
	uint32 m_BufferLength;
	uint32 m_WriteBufferIndex;
	uint32 m_ReadBufferIndex;
};

#endif