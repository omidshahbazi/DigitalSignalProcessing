#pragma once
#ifndef CHORUS_H
#define CHORUS_H

#include "Flanger.h"

template <typename T, uint32 SampleRate>
class Chorus : public Flanger<T, SampleRate>
{
public:
	Chorus(void)
		: m_WetRate(0)
	{
		Flanger<T, SampleRate>::SetTime(0.05);
		SetFeedback(SILENCE_GAIN_dB);
	}

	//[0, 100]
	void SetDepth(float Value)
	{
		ASSERT(0 <= Value && Value <= 100, "Invalid Value %f", Value);

		Flanger<T, SampleRate>::SetDepth(Value);
	}
	float GetDepth(void) const
	{
		return Flanger<T, SampleRate>::GetDepth();
	}

	//[1Hz, 3Hz]
	void SetRate(float Value)
	{
		ASSERT(0 < Value && Value <= 4, "Invalid Value %f", Value);

		Flanger<T, SampleRate>::SetRate(Value);
	}
	float GetRate(void) const
	{
		return Flanger<T, SampleRate>::GetRate();
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

protected:
	T Mix(T A, T B) override
	{
		return Math::ConstantPowerMix(A, B, m_WetRate);
	}

private:
	void SetFeedback(float Value)
	{
		Flanger<T, SampleRate>::SetFeedback(Value);
	}

private:
	float m_WetRate;
};

#endif
