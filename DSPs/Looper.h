#pragma once
#ifndef LOOPER_H
#define LOOPER_H

#include "IDSP.h"
#include "../Filters/BufferFilter.h"

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

	void SetIsPlaying(bool Value)
	{
		m_IsPlaying = Value;
		
		Log::WriteError("Looper", "SetIsPlaying %i", m_IsPlaying);
	}
	bool GetIsPlaying(void) const
	{
		return m_IsPlaying;
	}

	void SetIsRecording(bool Value)
	{
		ASSERT(m_IsPlaying, "Must be in Playing state to record");

		m_IsRecording = Value;

		if (m_IsRecording)
		{
			if (!m_MasterLineIsRecorded)
				m_Buffer.SetCurrentTime(0);
		}
		else
		{
			if (!m_MasterLineIsRecorded)
			{
				m_MasterLineIsRecorded = true;

				m_Buffer.SetTime(m_Buffer.GetCurrentTime());
			}

			m_Buffer.CopyTo(m_UndoBuffer);
		}
		
		Log::WriteError("Looper", "SetIsRecording %i %i %f %f", m_IsRecording, m_MasterLineIsRecorded, m_Buffer.GetTime(), m_Buffer.GetCurrentTime());
	}
	bool GetIsRecording(void) const
	{
		return m_IsRecording;
	}
	void ToggleRecording(void)
	{
		SetIsRecording(!m_IsRecording);
		
		Log::WriteError("Looper", "ToggleRecording");
	}

	void SetReverse(bool Value)
	{
		m_Buffer.SetReverse(Value);
		
		Log::WriteError("Looper", "SetReverse %i", Value);
	}
	bool GetReverse(void) const
	{
		return m_Buffer.GetReverse();
	}

	void Undo(void)
	{
		m_UndoBuffer.CopyTo(m_Buffer);
		
		Log::WriteError("Looper", "Undo");
	}

	void Clear(void)
	{
		m_UndoBuffer.Reset();

		m_Buffer.Reset();
		m_Buffer.SetTime(MaxTime);

		m_IsPlaying = true;
		m_MasterLineIsRecorded = false;
		
		Log::WriteError("Looper", "Clear");
	}

	//[0, 1]
	void SetOutputMixRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Buffer.SetOutputMixRate(Value);
		
		Log::WriteError("Looper", "SetOutputMixRate %f", Value);
	}
	float GetOutputMixRate(void) const
	{
		return m_Buffer.GetOutputMixRate();
	}

	//[-20dB, -0.5dB]
	void SetOverdubLevel(float Value)
	{
		ASSERT(-20 <= Value && Value <= -0.5, "Invalid Value %f", Value);

		m_Buffer.SetFeedback(0);
		
		Log::WriteError("Looper", "SetOverdubLevel %f", Value);
	}
	float GetOverdubLevel(void) const
	{
		return m_Buffer.GetFeedback();
	}

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		if (m_IsPlaying && m_IsRecording)
		{
			for (uint8 i = 0; i < Count; ++i)
				Buffer[i] = m_Buffer.Record(Buffer[i]);
		}
		else
			for (uint8 i = 0; i < Count; ++i)
				Buffer[i] = m_Buffer.Process(Buffer[i]);
		
	}

private:
	BufferFilter<T, SampleRate, MaxTime> m_UndoBuffer;
	BufferFilter<T, SampleRate, MaxTime> m_Buffer;
	bool m_IsPlaying;
	bool m_IsRecording;
	bool m_MasterLineIsRecorded;
};

#endif
