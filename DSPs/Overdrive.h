#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "../Math.h"
#include "../Debug.h"

template <typename T>
class Overdrive : public IDSP<T>
{
public:
	Overdrive(void)
		: m_Drive(0),
		  m_PreGain(0),
		  m_PostGain(0)
	{
		SetDrive(1);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_Drive = Value;

		m_Drive *= 2;

		const float sqrDrive = m_Drive * m_Drive;
		const float preGainA = m_Drive * 0.5;
		const float preGainB = sqrDrive * sqrDrive * m_Drive * 8;

		m_PreGain = preGainA + ((preGainB - preGainA) * sqrDrive);

		const float squashedDrive = m_Drive * (2 - m_Drive);

		m_PostGain = 1 / Math::SoftClip(0.33 + (squashedDrive * (m_PreGain - 0.33)));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = Math::SoftClip(Buffer[i] * m_PreGain) * m_PostGain;
	}

private:
	float m_Drive;
	float m_PreGain;
	float m_PostGain;
};

#endif