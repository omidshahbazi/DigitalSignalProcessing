#pragma once
#ifndef BUFFER_FILTER_H
#define BUFFER_FILTER_H

#include "Filter.h"
#include "../Math.h"
#include "../Memory.h"

template <typename T, uint32 SampleRate, uint16 MaxTime>
class BufferFilter : public Filter<T, SampleRate>
{
	static_assert(0 < MaxTime, "Invalid MaxTime");

public:
	BufferFilter(void)
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
		SetFeedback(0);
		SetOutputMixRate(0.5);
	}

	~BufferFilter(void)
	{
		Memory::Deallocate(m_Buffer);
	}

	//[0, MaxTime]
	void SetTime(float Value)
	{
		ASSERT(0 <= Value && Value <= MaxTime, "Invalid Value %f", Value);

		m_Time = Value;

		m_BufferLength = Math::Max(m_Time * SampleRate, 1);

		ASSERT(m_BufferLength < m_TotalBufferLength, "Exceeding total buffer length %i", m_BufferLength);
	}
	float GetTime(void) const
	{
		return m_Time;
	}

	void SetCurrentTime(float Value)
	{
		m_WriteBufferIndex = Value * SampleRate;

		ASSERT(m_WriteBufferIndex < m_BufferLength, "Exceeding buffer length %i", m_WriteBufferIndex);
		
		m_ReadBufferIndex = m_WriteBufferIndex;
	}
	float GetCurrentTime(void) const
	{
		return (float)m_WriteBufferIndex / SampleRate;
	}

	//[-120dB, 20dB]
	void SetFeedback(float Value)
	{
		ASSERT(-120 <= Value && Value <= 20, "Invalid Value %f", Value);

		m_Feedback = Value;
		m_FeedbackCoef = Math::dBToLinear(m_Feedback);
	}
	float GetFeedback(void) const
	{
		return m_Feedback;
	}

	//[0, 1]
	void SetOutputMixRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

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
		T delayedSample = GetCircularSample(m_ReadBufferIndex);

		MoveForward();
		
		return Math::Lerp(Value, delayedSample, m_OutputMixRate);
	}

	T Record(T Value)
	{
		T delayedSample = GetCircularSample(m_ReadBufferIndex);

		m_Buffer[m_WriteBufferIndex] = delayedSample + Value;

		MoveForward();

		return Math::Lerp(Value, delayedSample, m_OutputMixRate);
	}

	void Reset(void)
	{
		Memory::Set(m_Buffer, 0, m_TotalBufferLength);

		m_WriteBufferIndex = 0;
		m_ReadBufferIndex = 0;
	}

	void CopyTo(BufferFilter& Other, bool FullBuffer = false)
	{
		Memory::Copy(m_Buffer, Other.m_Buffer, FullBuffer ? m_TotalBufferLength : m_BufferLength);
	}

private:
	T GetCircularSample(uint32 Index) const
	{
		return m_Buffer[Math::Wrap(Index, 0, m_BufferLength - 1)] * m_FeedbackCoef;
	}

private:
	float m_Time;
	float m_Feedback;
	float m_OutputMixRate;
	bool m_Reverse;
	
	float m_FeedbackCoef;

	T *m_Buffer;
	uint32 m_TotalBufferLength;
	uint32 m_BufferLength;
	uint32 m_WriteBufferIndex;
	uint32 m_ReadBufferIndex;
};

#endif