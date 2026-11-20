#pragma once
#ifndef CHORUS_H
#define CHORUS_H

#include "Flanger.h"

template <typename T, uint32_t SampleRate>
class Chorus : public Flanger<T, SampleRate>
{
public:
	Chorus(void)
		: m_WetRate(0)
	{
		SetRate(Frequency(1.5));
		Flanger<T, SampleRate>::SetTime(50 ms);
		Flanger<T, SampleRate>::SetFeedback(SILENCE_GAIN);
	}
	//0-3ms
	void SetDepth(float Value)
	{
		Flanger<T, SampleRate>::SetDepth(Value);
	}
	float GetDepth(void) const
	{
		return Flanger<T, SampleRate>::GetDepth();
	}

	void SetRate(cc_Frequency<0, 3> Value)
	{
		Flanger<T, SampleRate>::SetRate(Value);
	}
	Frequency GetRate(void) const
	{
		return Flanger<T, SampleRate>::GetRate();
	}

	void SetWetRate(cc_ratio_t Value)
	{
		m_WetRate = Value;
	}
	float GetWetRate(void) const
	{
		return m_WetRate;
	}

protected:
	T Mix(T A, T B) override
	{
		return Math::AdditiveMix(A, B, m_WetRate);
	}

private:
	float m_WetRate;
};

#endif
