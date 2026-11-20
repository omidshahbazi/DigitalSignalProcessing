#pragma once
#ifndef AUTO_WAH_H
#define AUTO_WAH_H

#include "Wah.h"
#include "DigitalSignalProcessing/Filters/EnvelopeFollowerFilter.h"

template <typename T, uint32_t SampleRate>
class AutoWah : private Wah<T, SampleRate>
{
public:
	AutoWah(void)
	{
		SetSensitivity(1);
		m_EnvelopeFollowerFilter.SetAttackTime(15 ms);
		m_EnvelopeFollowerFilter.SetReleaseTime(100 ms);
	}

	void SetSensitivity(cc_ratio_t Value)
	{
		m_Sensitivity = Value;

		m_Multiplier = Math::Lerp(1, 20, m_Sensitivity);
	}
	float GetSensitivity(void) const
	{
		return m_Sensitivity;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		m_EnvelopeFollowerFilter.Process(Buffer, Count);
		Wah<T, SampleRate>::SetRatio(Math::Clamp01(m_EnvelopeFollowerFilter.GetCurrentEnvelope() * m_Multiplier));

		Wah<T, SampleRate>::Process(Buffer, Count);
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	float m_Sensitivity;
	float m_Multiplier;
};
#endif
