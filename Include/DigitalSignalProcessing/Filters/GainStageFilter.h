#pragma once
#ifndef GAIN_STAGE_FILTER_H
#define GAIN_STAGE_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Gain.h"

template<typename T, uint32_t SampleRate, typename Clipper>
class GainStageFilter : public Filter<T, SampleRate>
{
public:
	GainStageFilter(void)
	{
		SetInputGain(NORMAL_GAIN);
	}

	virtual void SetInputGain(LinearGain Value)
	{
		m_InputGain = Value;

		m_Clipper.SetInputGain(m_InputGain);
	}
	LinearGain GetInputGain(void) const
	{
		return m_InputGain;
	}

	Clipper& GetClipper(void)
	{
		return m_Clipper;
	}
	const Clipper& GetClipper(void) const
	{
		return m_Clipper;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= m_InputGain;

		m_Clipper.Process(Buffer, Count);
	}

	FILTER_FORWARD_DECLARATION()

private:
	LinearGain m_InputGain;

	Clipper m_Clipper;
};

#endif