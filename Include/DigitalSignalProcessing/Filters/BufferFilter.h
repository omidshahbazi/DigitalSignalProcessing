#pragma once
#ifndef BUFFER_FILTER_H
#define BUFFER_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Memory.h"

template <typename T, uint32_t SampleRate, uint16_t MaxTime>
class BufferFilter : public Filter<T, SampleRate>
{
	STATIC_ASSERT(0 < MaxTime, "Invalid MaxTime");

public:
	typedef cc_float<0, MaxTime> TimeRange;

public:
	BufferFilter(void)
		: m_Time(0),
		m_CurrentTime(0),
		m_Feedback(0),
		m_OutputMixRate(0),
		m_Reverse(false),
		m_Buffer(nullptr),
		m_TotalBufferLength(MaxTime* SampleRate),
		m_BufferLength(0),
		m_WriteBufferIndex(0),
		m_ReadBufferIndex(0),
		m_IsLastSample(false)
	{
		m_Buffer = Memory::Allocate<T>(m_TotalBufferLength, true);

		SetTime(MaxTime);
		SetFeedback(NORMAL_GAIN);
		SetOutputMixRate(0.5);
	}

	~BufferFilter(void)
	{
		Memory::Deallocate(m_Buffer);
	}

	void SetTime(TimeRange Value)
	{
		m_Time = Value;

		m_BufferLength = Math::Max(m_Time * SampleRate, 1);

		ASSERT_VALUE_LESS_EQUAL(m_BufferLength, m_TotalBufferLength);
	}
	float GetTime(void) const
	{
		return m_Time;
	}

	//[0, Time]
	void SetCurrentTime(TimeRange Value)
	{
		m_WriteBufferIndex = Value * SampleRate;

		ASSERT_VALUE_LESS(m_WriteBufferIndex, m_BufferLength);

		m_ReadBufferIndex = m_WriteBufferIndex;
	}
	float GetCurrentTime(void) const
	{
		return m_CurrentTime;
	}

	void SetFeedback(cc_dBGain<SILENCE_GAIN_VALUE, NORMAL_GAIN_VALUE> Value)
	{
		m_Feedback = Value;
		m_FeedbackCoef = m_Feedback;
	}
	dBGain GetFeedback(void) const
	{
		return m_Feedback;
	}

	void SetOutputMixRate(cc_ratio_t Value)
	{
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

	bool IsLastSample(void) const
	{
		return m_IsLastSample;
	}

	T GetSample(uint32_t Offset = 0) const
	{
		return GetCircularSample(m_ReadBufferIndex + Offset);
	}

	T GetLerpedSample(uint32_t Offset, float Fraction) const
	{
		uint32_t index = m_ReadBufferIndex + Offset;

		return Math::Lerp(GetCircularSample(index), GetCircularSample(index + 1), Fraction);
	}

	void MoveForward(void)
	{
		m_CurrentTime = (float)(m_WriteBufferIndex + 1) / SampleRate;
		m_IsLastSample = (m_WriteBufferIndex + 1 == m_BufferLength);

		m_WriteBufferIndex = Math::Wrap(m_WriteBufferIndex + 1, 0, m_BufferLength - 1);
		m_ReadBufferIndex = Math::Wrap(m_ReadBufferIndex + (m_Reverse ? -1 : 1), 0, m_BufferLength - 1);
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] = Process(Buffer[i]);
	}

	T Process(T Value) override
	{
		T delayedSample = GetCircularSample(m_ReadBufferIndex);

		MoveForward();

		return Math::LinearCrossFadeMix(Value, delayedSample, m_OutputMixRate);
	}

	T Record(T Value)
	{
		T delayedSample = GetCircularSample(m_ReadBufferIndex);

		m_Buffer[m_WriteBufferIndex] = (delayedSample * m_FeedbackCoef) + Value;

		MoveForward();

		return Math::LinearCrossFadeMix(Value, delayedSample, m_OutputMixRate);
	}

	void Reset(void)
	{
		m_WriteBufferIndex = 0;
		m_ReadBufferIndex = 0;
	}

	void Clear(void)
	{
		Memory::Set(m_Buffer, 0, m_TotalBufferLength);

		Reset();
	}

	void CopyTo(BufferFilter& Other, bool FullBuffer = false) const
	{
		Memory::Copy(m_Buffer, Other.m_Buffer, FullBuffer ? m_TotalBufferLength : m_BufferLength);
	}

private:
	T GetCircularSample(uint32_t Index) const
	{
		return m_Buffer[Math::Wrap(Index, 0, m_BufferLength - 1)];
	}

private:
	float m_Time;
	float m_CurrentTime;
	dBGain m_Feedback;
	float m_OutputMixRate;
	bool m_Reverse;

	LinearGain m_FeedbackCoef;

	T* m_Buffer;
	uint32_t m_TotalBufferLength;
	uint32_t m_BufferLength;
	uint32_t m_WriteBufferIndex;
	uint32_t m_ReadBufferIndex;
	bool m_IsLastSample;
};

#endif