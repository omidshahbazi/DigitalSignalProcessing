#pragma once
#ifndef LOOPER_H
#define LOOPER_H

#include "IDSP.h"
#include "../Filters/BufferFilter.h"
#include "../Filters/LowPassFilter.h"
#include "../Filters/HighPassFilter.h"

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

		m_Buffer.Reset();
	}
	bool GetIsPlaying(void) const
	{
		return m_IsPlaying;
	}
	void TogglePlaying(void)
	{
		SetIsPlaying(!m_IsPlaying);
	}

	void SetIsRecording(bool Value)
	{
		m_IsRecording = Value;

		if (m_IsRecording)
		{
			if (!m_MasterLineIsRecorded)
				m_Buffer.SetCurrentTime(0);

			m_Buffer.CopyTo(m_UndoBuffer);
		}
		else
		{
			if (!m_MasterLineIsRecorded)
			{
				m_MasterLineIsRecorded = true;

				m_Buffer.SetTime(m_Buffer.GetCurrentTime());
			}
		}
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
		m_UndoBuffer.Clear();

		m_Buffer.Clear();
		m_Buffer.SetTime(MaxTime);

		m_MasterLineIsRecorded = false;
	}

	//(0, MaxTime]
	void SetTime(float Value)
	{
		ASSERT(0 < Value && Value <= MaxTime, "Invalid Value %f", Value);

		m_Buffer.SetTime(Value);
	}
	float GetTime(void) const
	{
		return m_Buffer.GetTime();
	}

	//(0, Time]
	void SetCurrentTime(float Value)
	{
		ASSERT(0 < Value && Value <= GetTime(), "Invalid Value %f", Value);

		m_Buffer.SetCurrentTime(Value);
	}
	float GetCurrentTime(void) const
	{
		return m_Buffer.GetCurrentTime();
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

	//[-20dB, NORMAL_GAIN]
	void SetOverdubLevel(dBGain Value)
	{
		ASSERT(-20 <= Value && Value <= NORMAL_GAIN, "Invalid Value %f", Value);

		m_Buffer.SetFeedback(Value);
	}
	dBGain GetOverdubLevel(void) const
	{
		return m_Buffer.GetFeedback();
	}

	//[MIN_FREQUENCY, 200Hz]
	void SetLowCutFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= 200, "Invalid Value %f", Value);

		m_HighPassFilter.SetCutoffFrequency(Value);
	}
	Frequency GetLowCutFrequency(void) const
	{
		return m_HighPassFilter.GetLowCutFrequency();
	}

	//[5KHz, MAX_FREQUENCY]
	void SetHighCutFrequency(Frequency Value)
	{
		ASSERT(5 KHz <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_LowPassFilter.SetCutoffFrequency(Value);
	}
	Frequency GetHighCutFrequency(void) const
	{
		return m_HighPassFilter.GetHighCutFrequency();
	}

	bool GetIsLastSample(void) const
	{
		return m_Buffer.GetIsLastSample();
	}

	void Process(T *Buffer, uint8 Count) override
	{
		CLONE_BUFFER(filteredBuffer);

		m_HighPassFilter.Process(filteredBuffer, Count);
		m_LowPassFilter.Process(filteredBuffer, Count);

		for (uint8 i = 0; i < Count; ++i)
		{
			T output = 0;

			if (m_IsPlaying && m_IsRecording)
				output = m_Buffer.Record(filteredBuffer[i]);
			else
				output = m_Buffer.Process(filteredBuffer[i]);

			if (!m_IsPlaying)
				output = 0;

			Buffer[i] = Math::SoftClip(Mix(Buffer[i], output));
		}
	}

protected:
	T Mix(T A, T B) override
	{
		return Math::AdditiveMix(A, B, m_WetRate);
	}

private:
	BufferFilter<T, SampleRate, MaxTime> m_UndoBuffer;
	BufferFilter<T, SampleRate, MaxTime> m_Buffer;
	LowPassFilter<SampleType, SampleRate> m_LowPassFilter;
	HighPassFilter<SampleType, SampleRate> m_HighPassFilter;
	float m_WetRate;
	bool m_IsPlaying;
	bool m_IsRecording;
	bool m_MasterLineIsRecorded;
};

#endif
