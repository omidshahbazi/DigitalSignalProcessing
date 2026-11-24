#pragma once
#ifndef GAIN_STAGE_FILTER_H
#define GAIN_STAGE_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Gain.h"

template<typename T, uint32_t SampleRate, typename Clipper>
class GainStageFilter : public Filter<T, SampleRate>
{
public:
	GainStageFilter(void)
	{
		SetInputGain(NORMAL_GAIN);
	}

	virtual void SetInputGain(dBGain Value)
	{
		m_InputGain = Value;

		m_Gain = m_InputGain;
	}
	dBGain GetInputGain(void) const
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
			Buffer[i] *= m_Gain;

		m_Clipper.Process(Buffer, Count);
	}

	FILTER_FORWARD_DECLARATION()

private:
	dBGain m_InputGain;
	LinearGain m_Gain;

	Clipper m_Clipper;
};

#endif