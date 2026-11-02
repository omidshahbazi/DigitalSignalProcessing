#pragma once
#ifndef CLIPPER_FILTER_H
#define CLIPPER_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"

template<typename T, uint32_t SampleRate>
class ClipperFilter : public Filter<T, SampleRate>
{
public:
	ClipperFilter(void)
		: m_Asymmetry(0),
		m_PositiveThreshold(0),
		m_NegativeThreshold(0)
	{
		SetAsymmetry(0);
		SetPositiveThreshold(1);
		SetNegativeThreshold(-1);
	}

	virtual void SetInputGain(LinearGain Value)
	{}

	//[0, 1]
	virtual void SetAsymmetry(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Asymmetry = Value;
	}
	float GetAsymmetry(void) const
	{
		return m_Asymmetry;
	}

	//[0, 1]
	virtual void SetPositiveThreshold(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_PositiveThreshold = Value;
	}
	float GetPositiveThreshold(void) const
	{
		return m_PositiveThreshold;
	}

	//[0, 1]
	virtual void SetNegativeThreshold(float Value)
	{
		ASSERT(-1 <= Value && Value <= 0, "Invalid Value %f", Value);

		m_NegativeThreshold = Value;
	}
	float GetNegativeThreshold(void) const
	{
		return m_NegativeThreshold;
	}

	FILTER_FORWARD_DECLARATION()

private:
	float m_Asymmetry;
	T m_PositiveThreshold;
	T m_NegativeThreshold;
};

#endif