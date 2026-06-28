#pragma once
#ifndef CLIPPER_DIODE_FILTER_H
#define CLIPPER_DIODE_FILTER_H

#include "Filter.h"
#include "../Math.h"

enum class ClipperDiodeFilterTypes : uint8
{
	Silicon = 0,
	Germanium,
	RedLED
};

template<typename T, uint32 SampleRate, ClipperDiodeFilterTypes Type>
class ClipperDiodeFilter : public Filter<T, SampleRate>
{
public:
	ClipperDiodeFilter(void)
	{
		if constexpr (Type == ClipperDiodeFilterTypes::Silicon)
		{
			//	m_ForwardVoltage = 0.7;
			//	m_LeakageFactor = 0.01;
			//	m_Softness = 0.5;

			m_ForwardVoltage = 0.7;
			m_LeakageFactor = 0.01;
			m_Softness = 1;
		}
		else if constexpr (Type == ClipperDiodeFilterTypes::Germanium)
		{
			//	m_ForwardVoltage = 0.3;
			//	m_LeakageFactor = 0.01;
			//	m_Softness = 0.5;

			m_ForwardVoltage = 0.3;
			m_LeakageFactor = 0.05;
			m_Softness = 0.8;
		}
		else if constexpr (Type == ClipperDiodeFilterTypes::RedLED)
		{
			//	m_ForwardVoltage = 1.8;
			//	m_LeakageFactor = 0.01;
			//	m_Softness = 0.5;

			m_ForwardVoltage = 1.8;
			m_LeakageFactor = 0.001;
			m_Softness = 1.2;
		}
	}

	void Process(T* Buffer, uint8 Count) override
	{
		for (uint8 i = 0; i < Count; ++i)
		{
			T leakedInput = Buffer[i] + (Buffer[i] * m_LeakageFactor);

			Buffer[i] = Math::SoftClip((leakedInput * m_Softness) / m_ForwardVoltage, 0);
		}
	}

	FILTER_FORWARD_DECLARATION()

private:
	// The threshold voltage where clipping begins. 
	// Higher values provide more headroom, lower values clip earlier.
	// Typical range: 0.1f (Low) to 2.0f (High)
	float m_ForwardVoltage;

	// Simulates the non-ideal reverse leakage current of the diode. 
	// Higher values create a "leaky" or less defined transition.
	// Typical range: 0.0f to 0.1f
	float m_LeakageFactor;

	// Controls the "knee" transition smoothness. 
	// Low values = smooth/soft compression (tube-like); High values = sharp/hard clipping.
	// Typical range: 0.1f to 5.0f
	float m_Softness;
};

template<typename T, uint32 SampleRate>
using SiliconClipperDiodeFilter = ClipperDiodeFilter<T, SampleRate, ClipperDiodeFilterTypes::Silicon>;

template<typename T, uint32 SampleRate>
using GermaniumClipperDiodeFilter = ClipperDiodeFilter<T, SampleRate, ClipperDiodeFilterTypes::Germanium>;

template<typename T, uint32 SampleRate>
using SiliconClipperDiodeFilter = ClipperDiodeFilter<T, SampleRate, ClipperDiodeFilterTypes::Silicon>;

#endif