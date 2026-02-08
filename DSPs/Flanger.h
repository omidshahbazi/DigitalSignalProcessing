#pragma once
#ifndef FLANGER_H
#define FLANGER_H

#include "Chorus.h"

template <typename T, uint32 SampleRate>
class Flanger : public Chorus<T, SampleRate>
{
public:
	Flanger(void)
	{
		SetFeedback(-6.02);
	}

	//[-120dB, 0dB]
	void SetFeedback(float Value)
	{
		ASSERT(-120 <= Value && Value <= 0, "Invalid Value %f", Value);

		Chorus<T, SampleRate>::m_Buffer.SetFeedback(Value);
	}
	float GetFeedback(void) const
	{
		return Chorus<T, SampleRate>::m_Buffer.GetFeedback();
	}
};

#endif
