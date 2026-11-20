#pragma once
#ifndef LOOPER_H
#define LOOPER_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Filters/BufferFilter.h"
#include "DigitalSignalProcessing/Filters/LowPassFilter.h"
#include "DigitalSignalProcessing/Filters/HighPassFilter.h"

template <typename T, uint32_t SampleRate, uint16_t MaxTime>
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
	bool IsPlaying(void) const
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
	bool IsRecording(void) const
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

	void SetTime(oc_float<0, MaxTime> Value)
	{
		m_Buffer.SetTime(Value);
	}
	float GetTime(void) const
	{
		return m_Buffer.GetTime();
	}

	//(0, Time]
	void SetCurrentTime(float Value)
	{
		ASSERT_VALUE_RANGE_MAX_INCLUSIVE(Value, 0, GetTime());

		m_Buffer.SetCurrentTime(Value);
	}
	float GetCurrentTime(void) const
	{
		return m_Buffer.GetCurrentTime();
	}

	void SetWetRate(cc_ratio_t Value)
	{
		m_WetRate = Value;
	}
	float GetWetRate(void) const
	{
		return m_WetRate;
	}

	void SetOverdubLevel(cc_dBGain<-20, NORMAL_GAIN_VALUE> Value)
	{
		m_Buffer.SetFeedback(Value);
	}
	dBGain GetOverdubLevel(void) const
	{
		return m_Buffer.GetFeedback();
	}

	void SetLowCutFrequency(cc_Frequency<MIN_FREQUENCY_VALUE, 200> Value)
	{
		m_HighPassFilter.SetCutoffFrequency(Value);
	}
	Frequency GetLowCutFrequency(void) const
	{
		return m_HighPassFilter.GetLowCutFrequency();
	}

	void SetHighCutFrequency(cc_Frequency<int16_t(5 KHz), MAX_FREQUENCY_VALUE> Value)
	{
		m_LowPassFilter.SetCutoffFrequency((Frequency)Value);
	}
	Frequency GetHighCutFrequency(void) const
	{
		return m_LowPassFilter.GetHighCutFrequency();
	}

	bool IsLastSample(void) const
	{
		return m_Buffer.IsLastSample();
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		CLONE_BUFFER(filteredBuffer);

		m_HighPassFilter.Process(filteredBuffer, Count);
		m_LowPassFilter.Process(filteredBuffer, Count);

		for (uint8_t i = 0; i < Count; ++i)
		{
			T output = 0;

			if (m_IsPlaying && m_IsRecording)
				output = m_Buffer.Record(filteredBuffer[i]);
			else
				output = m_Buffer.Process(filteredBuffer[i]);

			if (!m_IsPlaying)
				output = 0;

			Buffer[i] = Mix(Buffer[i], output);
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
	LowPassFilter<T, SampleRate> m_LowPassFilter;
	HighPassFilter<T, SampleRate> m_HighPassFilter;
	float m_WetRate;
	bool m_IsPlaying;
	bool m_IsRecording;
	bool m_MasterLineIsRecorded;
};

#endif
