#pragma once
#ifndef SAMPLE_AMOUNT_METER_H
#define SAMPLE_AMOUNT_METER_H

#include "Common.h"
#include "Debug.h"
#include <memory.h>

class SampleAmountMeter
{
public:
	SampleAmountMeter(void)
		: m_SampleCount(0),
		  m_SampleSum(0),
		  m_Average(0),
		  m_Min(0),
		  m_Max(0)
	{
	}

	void Record(float Value)
	{
		++m_SampleCount;

		m_SampleSum += Value;

		m_Average = m_SampleSum / m_SampleCount;

		if (Value < m_Min)
			m_Min = Value;
		else if (Value > m_Max)
			Value = m_Max;
	}

	float GetMin(void) const
	{
		return m_Min;
	}

	float GetMax(void) const
	{
		return m_Max;
	}

	float GetAverage(void) const
	{
		return m_Average;
	}

	void Reset(void)
	{
		m_SampleCount = 0;
		m_SampleSum = 0;
		m_Average = 0;
		m_Min = 0;
		m_Max = 0;
	}

private:
	uint32 m_SampleCount;
	float m_SampleSum;
	float m_Average;
	float m_Min;
	float m_Max;
};
#endif