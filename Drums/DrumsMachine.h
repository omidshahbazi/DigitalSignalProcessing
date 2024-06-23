#pragma once
#ifndef DRUMS_MATCHINE_H
#define DRUMS_MATCHINE_H

#include "../IHAL.h"
#include "Kick.h"
#include "Snare.h"
#include "Cymbal.h"
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

	enum class Parts
	{
		None = 0b0000000,
		Kick = 0b00000001,
		Snare = 0b00000010,
		CymbalHiHat = 0b00000100,
		CymbalRide = 0b00001000,
		CymbalCrash = 0b00010000,
		TomFloor = 0b00100000,
		TomMiddle = 0b01000000,
		TomHigh = 0b10000000
	};

private:
	static constexpr uint8 NOTES_COUNT = 8;

public:
	DrumsMachine(IHAL *HAL)
		: m_HAL(HAL),
		  m_EnabledParts(Parts::None),
		  m_NoteDuration(NoteDurations::Whole),
		  m_BeatsPerMinute(0),
		  m_BeatTime(0),
		  m_NextBeatTime(0),
		  m_Pattern(nullptr),
		  m_PatternLength(0),
		  m_PatternIndex(0),
		  m_Parts{}
	{
		SetBeatsPerMinute(60);

		m_CymbalHiHat.SetType(Cymbal<T, SampleRate>::Types::HiHat);
		m_CymbalRide.SetType(Cymbal<T, SampleRate>::Types::Ride);
		m_CymbalCrash.SetType(Cymbal<T, SampleRate>::Types::Crash);

		m_TomFloor.SetType(Tom<T, SampleRate>::Types::Floor);
		m_TomMiddle.SetType(Tom<T, SampleRate>::Types::Middle);
		m_TomHigh.SetType(Tom<T, SampleRate>::Types::High);

		uint8 index = 0;
		m_Parts[index++] = &m_Kick;
		m_Parts[index++] = &m_Snare;
		m_Parts[index++] = &m_CymbalHiHat;
		m_Parts[index++] = &m_CymbalRide;
		m_Parts[index++] = &m_CymbalCrash;
		m_Parts[index++] = &m_TomFloor;
		m_Parts[index++] = &m_TomMiddle;
		m_Parts[index++] = &m_TomHigh;
	}

	void SetEnabledParts(Parts Value)
	{
		m_EnabledParts = Value;
	}

	Parts GetEnabledParts(void) const
	{
		return m_EnabledParts;
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

	void SetNotes(Parts *Parts, uint8 Length)
	{
		m_Pattern = Parts;
		m_PatternLength = Length;
	}

	void Update(void)
	{
		if (m_PatternLength == 0)
			return;

		uint32 time = m_HAL->GetTimeSinceStartupMs();
		if (time >= m_NextBeatTime)
		{
			Parts parts = m_Pattern[m_PatternIndex];
			m_PatternIndex = Math::Moderate(m_PatternIndex + 1, m_PatternLength);

			for (uint8 i = 0; i < sizeof(uint8) * 8; ++i)
			{
				uint8 id = (1 << i);

				if (((uint8)m_EnabledParts & id) == 0)
					continue;

				if (((uint8)parts & id) == 0)
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
	Parts m_EnabledParts;
	NoteDurations m_NoteDuration;
	float m_BeatsPerMinute;
	uint32 m_BeatTime;
	uint32 m_NextBeatTime;
	Parts *m_Pattern;
	uint8 m_PatternLength;
	uint8 m_PatternIndex;
	DrumsPart<T, SampleRate> *m_Parts[NOTES_COUNT];

	Kick<T, SampleRate> m_Kick;
	Snare<T, SampleRate> m_Snare;
	Cymbal<T, SampleRate> m_CymbalHiHat;
	Cymbal<T, SampleRate> m_CymbalRide;
	Cymbal<T, SampleRate> m_CymbalCrash;
	Tom<T, SampleRate> m_TomFloor;
	Tom<T, SampleRate> m_TomMiddle;
	Tom<T, SampleRate> m_TomHigh;
};

#endif