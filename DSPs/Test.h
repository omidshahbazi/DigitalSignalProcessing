#pragma once
#ifndef TEST_H
#define TEST_H

#include "IDSP.h"
#include "../Filters/TripleToneControlFilter.h"

template <typename T>
class Test : public IDSP<T>
{
public:
	Test(uint32 SampleRate)
		: m_TripleToneControlFilter(SampleRate)
	{
	}

	void SetLowTone(float Value)
	{
		m_TripleToneControlFilter.SetLowTone(Value);
	}

	void SetMidTone(float Value)
	{
		m_TripleToneControlFilter.SetMidTone(Value);
	}

	void SetHighTone(float Value)
	{
		m_TripleToneControlFilter.SetHighTone(Value);
	}

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = m_TripleToneControlFilter.Process(Buffer[i]);
	}

private:
	TripleToneControlFilter<T> m_TripleToneControlFilter;
};

#endif