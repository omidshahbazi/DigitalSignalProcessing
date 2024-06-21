#pragma once
#ifndef DRUMS_MATCHINE_H
#define DRUMS_MATCHINE_H

#include "../IHAL.h"
#include "Kick.h"
#include "Snare.h"
#include "HiHat.h"
#include "Tom.h"

template <typename T, uint32 SampleRate>
class DrumsMachine
{
	ASSERT_ON_FLOATING_TYPE(T);
	ASSERT_ON_SAMPLE_RATE(SampleRate);

public:
	enum class NoteDurations
	{
		Whole = 1,
		Half = 2,
		Quarter = 4,
		Eighth = 8,
		Sixteenth = 16,
		ThirtySecond = 32
	};

	enum class Notes
	{
		Kick = 0b00000001,
		Snare = 0b00000010,
		HiHat = 0b00000100,
		Tom = 0b00001000
	};

private:
	static constexpr uint8 NOTES_COUNT = 4;

public:
	DrumsMachine(IHAL *HAL)
		: m_HAL(HAL),
		  m_NoteDuration(NoteDurations::Whole),
		  m_BeatsPerMinute(0),
		  m_BeatTime(0),
		  m_NextBeatTime(0),
		  m_Notes(nullptr),
		  m_NotesCount(0),
		  m_NotesIndex(0),
		  m_Parts{}
	{
		SetBeatsPerMinute(60);

		uint8 index = 0;
		m_Parts[index++] = &m_Kick;
		m_Parts[index++] = &m_Snare;
		m_Parts[index++] = &m_HiHat;
		m_Parts[index++] = &m_Tom;
	}

	void SetNoteDuration(NoteDurations Value)
	{
		m_NoteDuration = Value;

		UpdateData();
	}
	NoteDurations GetNoteDuration(void) const
	{
		return m_NoteDuration;
	}

	//[20, 400]
	void SetBeatsPerMinute(float Value)
	{
		m_BeatsPerMinute = Value;

		UpdateData();
	}
	float GetBeatsPerMinute(void) const
	{
		return m_BeatsPerMinute;
	}

	void SetNotes(Notes *Notes, uint8 Count)
	{
		m_Notes = Notes;
		m_NotesCount = Count;
	}

	void Update(void)
	{
		if (m_NotesCount == 0)
			return;

		uint32 time = m_HAL->GetTimeSinceStartupMs();
		if (time >= m_NextBeatTime)
		{
			Notes notes = m_Notes[m_NotesIndex];
			m_NotesIndex = Math::Moderate(m_NotesIndex + 1, m_NotesCount);

			for (uint8 i = 0; i < sizeof(uint8) * 8; ++i)
			{
				if (((uint8)notes & (1 << i)) == 0)
					continue;

				m_Parts[i]->Beat();
			}

			m_NextBeatTime = time + m_BeatTime;
		}
	}

	T Process(void)
	{
		T samplesSum = 0;
		for (uint8 j = 0; j < NOTES_COUNT; ++j)
			samplesSum += m_Parts[j]->Process();

		return samplesSum / NOTES_COUNT;
	}

private:
	void UpdateData(void)
	{
		m_BeatTime = 60 * 1000 / (m_BeatsPerMinute * (uint8)m_NoteDuration);
		m_NextBeatTime = m_HAL->GetTimeSinceStartupMs() + m_BeatTime;
	}

private:
	IHAL *m_HAL;
	NoteDurations m_NoteDuration;
	float m_BeatsPerMinute;
	uint32 m_BeatTime;
	uint32 m_NextBeatTime;
	Notes *m_Notes;
	uint8 m_NotesCount;
	uint8 m_NotesIndex;
	DrumsPart<T, SampleRate> *m_Parts[NOTES_COUNT];

	Kick<T, SampleRate> m_Kick;
	Snare<T, SampleRate> m_Snare;
	HiHat<T, SampleRate> m_HiHat;
	Tom<T, SampleRate> m_Tom;
};

#endif