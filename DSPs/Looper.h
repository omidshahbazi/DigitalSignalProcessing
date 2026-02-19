#pragma once
#ifndef LOOPER_H
#define LOOPER_H

#include "IDSP.h"
#include "../Filters/BufferFilter.h"

template <typename T, uint32 SampleRate, uint16 MaxTime>
class Looper : public IDSP<T, SampleRate>
{
public:
	Looper(void)
		: m_WetRate(0),
		  m_IsPlaying(true),
		  m_IsRecording(false),
		  m_MasterLineIsRecorded(false)
	{
		m_Buffer.SetOutputMixRate(1);
	}

	void SetIsPlaying(bool Value)
	{
		m_IsPlaying = Value;
	}
	bool GetIsPlaying(void) const
	{
		return m_IsPlaying;
	}

	void SetIsRecording(bool Value)
	{
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
	void SetWetRate(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_WetRate = Value;
	}
	float GetWetRate(void) const
	{
		return m_WetRate;
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
		for (uint8 i = 0; i < Count; ++i)
		{
			T output = 0;

			if (m_IsPlaying && m_IsRecording)
				output = m_Buffer.Record(Buffer[i]);
			else
				output = m_Buffer.Process(Buffer[i]);

			Buffer[i] = Mix(Buffer[i], output);
		}
	}

protected:
	T Mix(T A, T B) override
	{
		return Math::ConstantPowerMix(A, B, m_WetRate);
	}

private:
	BufferFilter<T, SampleRate, MaxTime> m_UndoBuffer;
	BufferFilter<T, SampleRate, MaxTime> m_Buffer;
	float m_WetRate;
	bool m_IsPlaying;
	bool m_IsRecording;
	bool m_MasterLineIsRecorded;
};

#endif
