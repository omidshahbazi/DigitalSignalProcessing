#pragma once
#ifndef LOOPER_H
#define LOOPER_H

#include "IDSP.h"
#include "../Filters/BufferFilter.h"

//How it should work
//https://www.youtube.com/watch?v=b-tNt3fUVAY
template <typename T, uint32 SampleRate, uint16 MaxTime>
class Looper : public IDSP<T, SampleRate>
{
public:

public:
	Looper(void)
		: m_IsPlaying(true),
		  m_IsRecording(false),
		  m_MasterLineIsRecorded(false)
	{
		SetOutputMixRate(0.5);
	}

	void SetPlaying(bool Value)
	{
		m_IsPlaying = Value;
	}
	bool GetPlaying(void) const
	{
		return m_IsPlaying;
	}

	void SetRecording(bool Value)
	{
		if (m_IsRecording && !Value)
		{
			m_MasterLineIsRecorded = true;

			m_Buffer.HookTime();
		}

		m_Buffer.CopyTo(m_UndoBuffer);
		
		m_IsRecording = Value;
	}
	bool GetRecording(void) const
	{
		return m_IsRecording;
	}

	void SetReverse(bool Value)
	{
		m_Buffer.SetReverse(Value);
	}
	bool GetReverse(void) const
	{
		return m_Buffer.GetReverse();
	}

	void Undo(void)
	{
		m_UndoBuffer.CopyTo(m_Buffer);
	}

	void Clear(void)
	{
		m_UndoBuffer.Reset();

		m_Buffer.Reset();
		m_Buffer.SetTime(MaxTime);

		m_IsPlaying = true;
		m_MasterLineIsRecorded = false;
	}

	//[0, 1]
	void SetOutputMixRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Buffer.SetOutputMixRate(Value);
	}
	float GetOutputMixRate(void) const
	{
		return m_Buffer.GetOutputMixRate();
	}

	//[-20dB, -0.5dB]
	void SetOverdubLevel(float Value)
	{
		ASSERT(-20 <= Value && Value <= -0.5, "Invalid Value %f", Value);

		m_Buffer.SetFeedback(Value);
	}
	float GetOverdubLevel(void) const
	{
		return m_Buffer.GetFeedback();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		if (m_IsPlaying)
			for (uint16 i = 0; i < Count; ++i)
				Buffer[i] = m_Buffer.Process(Buffer[i]);
		else
			for (uint16 i = 0; i < Count; ++i)
				Buffer[i] = m_Buffer.Record(Buffer[i]);
	}

private:
	BufferFilter<T, SampleRate, MaxTime> m_UndoBuffer;
	BufferFilter<T, SampleRate, MaxTime> m_Buffer;
	bool m_IsPlaying;
	bool m_IsRecording;
	bool m_MasterLineIsRecorded;
};

#endif
