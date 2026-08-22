#pragma once
#ifndef AUTO_WAH_H
#define AUTO_WAH_H

#include "Wah.h"
#include "../Filters/EnvelopeFollowerFilter.h"

template <typename T, uint32 SampleRate>
class AutoWah : private Wah<T, SampleRate>
{
public:
	AutoWah(void)
		: m_Sensitivity(1)
	{
		m_EnvelopeFollowerFilter.SetAttackTime(15 ms);
		m_EnvelopeFollowerFilter.SetReleaseTime(100 ms);
	}

	//[1, 20]
	void SetSensitivity(float Value)
	{
		ASSERT(0 < Value && Value <= 20, "Invalid Value %f", Value);

		m_Sensitivity = Value;
	}
	float GetSensitivity(void) const
	{
		return m_Sensitivity;
	}

	void Process(T *Buffer, uint8 Count) override
	{
		m_EnvelopeFollowerFilter.Process(Buffer, Count);
		Wah<T, SampleRate>::SetRatio(Math::Clamp01(m_EnvelopeFollowerFilter.GetCurrentEnvelope() * m_Sensitivity));

		Wah<T, SampleRate>::Process(Buffer, Count);
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	float m_Sensitivity;
};
#endif
