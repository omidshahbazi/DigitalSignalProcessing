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
		: m_Gain(0),
		  m_Drive(0),
		  m_Factor(0),
		  m_Multiplier(0)
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
		const float preGainB = sqrDrive * sqrDrive * m_Drive * 24;

		m_PreGain = preGainA + (preGainB - preGainA) * sqrDrive;

		const float squashedDrive = m_Drive * (2 - m_Drive);

		m_PostGain = 1 / Math::SoftClip3(0.33 + squashedDrive * (m_PreGain - 0.33));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	void ProcessBuffer(T *Buffer, uint16 Count) override
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = Math::SoftClip3(Buffer[i] * m_PreGain) * m_PostGain;
	}

private:
	float m_Drive;
	float m_PreGain;
	float m_PostGain;
};

#endif

// #pragma once
// #ifndef OVERDRIVE_H
// #define OVERDRIVE_H

// #include "IDSP.h"
// #include "../Math.h"
// #include "../Debug.h"

// template <typename T>
// class Overdrive : public IDSP<T>
// {
// public:
// 	Overdrive(void)
// 		: m_Gain(0),
// 		  m_Drive(0),
// 		  m_Factor(0),
// 		  m_Multiplier(0)
// 	{
// 		SetGain(1);
// 		SetDrive(1);
// 	}

// 	//[0, 1]
// 	void SetGain(float Value)
// 	{
// 		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

// 		m_Gain = Value;

// 		m_Factor = Math::Lerp(2.0, 4.0, m_Gain);
// 	}
// 	float GetGain(void) const
// 	{
// 		return m_Gain;
// 	}

// 	//[0, 1]
// 	void SetDrive(float Value)
// 	{
// 		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

// 		m_Drive = Value;

// 		m_Multiplier = Math::Lerp(100.0, 200, m_Drive);
// 	}
// 	float GetDrive(void) const
// 	{
// 		return m_Drive;
// 	}

// 	void ProcessBuffer(T *Buffer, uint16 Count) override
// 	{
// 		for (uint16 i = 0; i < Count; ++i)
// 			Buffer[i] = Math::SoftClip(Buffer[i] * m_Multiplier, m_Factor) / m_Multiplier;
// 	}

// private:
// 	float m_Gain;
// 	float m_Drive;

// 	float m_Factor;
// 	float m_Multiplier;
// };

// #endif