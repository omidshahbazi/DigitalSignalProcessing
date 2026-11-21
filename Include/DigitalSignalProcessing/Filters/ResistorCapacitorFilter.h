#pragma once
#ifndef RESISTOR_CAPACITOR_FILTER_H
#define RESISTOR_CAPACITOR_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"

enum class ResistorCapacitorOrders : uint8_t
{
	ResistorFirst = 0,
	CapacitorFirst,

	LowPassMode = ResistorFirst,
	HighPassMode = CapacitorFirst
};

template<typename T, uint32_t SampleRate, ResistorCapacitorOrders Order>
class ResistorCapacitorFilter : public Filter<T, SampleRate>
{
	STATIC_ASSERT(SampleRate > 2 * MAX_FREQUENCY_VALUE, "SampleRate must be above twice MAX_FREQUENCY");

public:
	ResistorCapacitorFilter(void)
		: m_Alpha(0),
		  m_Memory(0)
	{
		SetCutoffFrequency(MIN_FREQUENCY);
	}

	void SetCutoffFrequency(Frequency_hearable Value)
	{
		m_CutoffFrequency = Value;

		const T g = Math::Tan(Math::PI_VALUE * m_CutoffFrequency / SampleRate);

		m_Alpha = g / (1 + g);
	}
	Frequency GetCutoffFrequency(void) const
	{
		return m_CutoffFrequency;
	}

	void Reset(void)
	{
		m_Memory = 0;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			const T v = (Buffer[i] - m_Memory) * m_Alpha;
			const T lp = v + m_Memory;
			m_Memory = lp + v;

			if constexpr (Order == ResistorCapacitorOrders::ResistorFirst)
				Buffer[i] = lp;
			else if constexpr (Order == ResistorCapacitorOrders::CapacitorFirst)
				Buffer[i] -= lp;
			else
				NOT_IMPLEMENTED();
		}
	}

	FILTER_FORWARD_DECLARATION()

private:
	Frequency m_CutoffFrequency;
	T m_Alpha;
	T m_Memory;
};

template<typename T, uint32_t SampleRate>
using LowPassResistorCapacitorFilter = ResistorCapacitorFilter<T, SampleRate, ResistorCapacitorOrders::LowPassMode>;

template<typename T, uint32_t SampleRate>
using HighPassResistorCapacitorFilter = ResistorCapacitorFilter<T, SampleRate, ResistorCapacitorOrders::HighPassMode>;

#endif