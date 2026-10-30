#pragma once
#ifndef TRANSISTOR_CLIPPER_FILTER_H
#define TRANSISTOR_CLIPPER_FILTER_H

#include "Filter.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Gain.h"

template<typename T, uint32_t SampleRate>
class TransistorClipperFilter : public Filter<T, SampleRate>
{
public:
	TransistorClipperFilter(void)
		: m_Drive(0),
		m_SquareCoeff(0),
		m_CubeCoeff(0)
	{
		SetDrive(0.5f);
	}

	//[0, 1]
	void SetParameter(float Value)
	{
		SetDrive(Value);
	}
	float GetParameter(void) const
	{
		return GetDrive();
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Drive = Value;

		m_InputGain = (LinearGain)Math::Lerp(1.0, 35.0, m_Drive);
		m_SquareCoeff = Math::Lerp(0.1, 0.5, m_Drive);
		m_CubeCoeff = Math::Lerp(0.05, 0.3, m_Drive);
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			float x = Buffer[i] * m_InputGain;
			float absX = Math::Absolute(x);

			float asymmetricFactor = (x >= 0) ? 1 : 0.7;

			float squared = x * absX * asymmetricFactor;
			float cubed = x * x * x;

			float processed = x - (m_SquareCoeff * squared * asymmetricFactor) - (m_CubeCoeff * cubed);

			Buffer[i] = Math::TanH(processed);
		}
	}

private:
	float m_Drive;
	LinearGain m_InputGain;
	float m_SquareCoeff;
	float m_CubeCoeff;
};

#endif