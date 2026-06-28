#pragma once
#ifndef RESISTOR_CAPACITOR_FILTER_H
#define RESISTOR_CAPACITOR_FILTER_H

#include "Filter.h"
#include "../Math.h"

enum class ResistorCapacitorOrders : uint8
{
	ResistorFirst = 0,
	CapacitorFirst,

	LowPassMode = ResistorFirst,
	HighPassMode = CapacitorFirst
};

template<typename T, uint32 SampleRate, ResistorCapacitorOrders Order>
class ResistorCapacitorFilter : public Filter<T, SampleRate>
{
public:
	ResistorCapacitorFilter(void)
		: m_Alpha(0),
		  m_Memory(0)
	{
		SetCutoffFrequency(MIN_FREQUENCY);
	}

	void SetCutoffFrequency(Frequency Value)
	{
		ASSERT(MIN_FREQUENCY <= Value && Value <= MAX_FREQUENCY, "Invalid Value %f", Value);

		m_CutoffFrequency = Value;

		m_Alpha = 1 / (1 + (SampleRate / (Math::TWO_PI_VALUE * m_CutoffFrequency)));

		Reset();
	}
	Frequency GetCutoffFrequency(void) const
	{
		return m_CutoffFrequency;
	}

	void Reset(void)
	{
		m_Memory = 0;
	}

	void Process(T* Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			m_Memory += m_Alpha * (Buffer[i] - m_Memory);

			if constexpr (Order == ResistorCapacitorOrders::ResistorFirst)
			{
				Buffer[i] = m_Memory;
			}
			else if constexpr (Order == ResistorCapacitorOrders::CapacitorFirst)
			{
				Buffer[i] -= m_Memory;
			}
		}
	}

	FILTER_FORWARD_DECLARATION()

private:
	Frequency m_CutoffFrequency;
	T m_Alpha;
	T m_Memory;
};

template<typename T, uint32 SampleRate>
using LowPassResistorCapacitorFilter = ResistorCapacitorFilter<T, SampleRate, ResistorCapacitorOrders::LowPassMode>;

template<typename T, uint32 SampleRate>
using HighPassResistorCapacitorFilter = ResistorCapacitorFilter<T, SampleRate, ResistorCapacitorOrders::HighPassMode>;

#endif