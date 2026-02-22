#pragma once
#ifndef FUZZ_H
#define FUZZ_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"
#include "../Filters/WaveShaperFilter.h"
#include "../Filters/LowPassFilter.h"

template <typename T, uint32 SampleRate>
class Fuzz : public IDSP<T, SampleRate>
{
public:
	Fuzz(void)
		: m_Level(0),
		  m_WetRate(0),
		  m_PreGain(0),
		  m_PrevAbsValue(0)
	{
		static typename WaveShaperFilter<T>::TablePoints points[] = {{-1, -1}, {-0.4, -1}, {0, 0}, {0.4, 1}, {1, 1}};
		m_WaveShaperFilter.SetTable(points, 5);

		SetTone(0.5);
		SetLevel(0);
		SetWetRate(0.5);
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Tone = Value;

		m_Filter.SetCutoffFrequency(Math::Lerp(1.0 KHz, 5 KHz, m_Tone));
		m_Filter.SetQualityFactory(0.3);
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	//[0, 1]
	void SetLevel(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Level = Value;

		m_PreGain = Math::dBToLinear(Math::Lerp(10.0, 30, m_Level));
	}
	float GetLevel(void) const
	{
		return m_Level;
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

	void ProcessBuffer(T *Buffer, uint8 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
		{
			T value = Buffer[i];

			value = m_Filter.Process(value);

			T prevAbsValue = m_PrevAbsValue;
			m_PrevAbsValue = Math::Absolute(value);
			if (Math::Absolute(value) >= prevAbsValue)
				value = 0;

			value *= m_PreGain;

			value = Math::Lerp(Buffer[i], value, m_WetRate);

			value = Math::Clamp(value, -1, 1);

			Buffer[i] = value;
		}
	}

private:
	WaveShaperFilter<T> m_WaveShaperFilter;
	LowPassFilter<T, SampleRate> m_Filter;

	float m_Tone;
	float m_Level;
	float m_WetRate;

	float m_PreGain;

	T m_PrevAbsValue;
};

#endif