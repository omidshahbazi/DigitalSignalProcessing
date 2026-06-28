#pragma once
#ifndef SIGNAL_CLIPPER_FILTER_H
#define SIGNAL_CLIPPER_FILTER_H

#include "ResistorCapacitorFilter.h"
#include "ClipperDiodeFilter.h"

template <typename T, uint32 SampleRate, ClipperDiodeFilterTypes DiodeType>
class SignalClipperFilter : public Filter<T, SampleRate>
{
public:
	SignalClipperFilter(void)
		: m_AsymmetryLevel(0)
	{
		m_HighPassFilter.SetCutoffFrequency(Frequency(80));
	}

	//[NORMAL_GAIN, 12dB]
	void SetGain(dBGain Value)
	{
		ASSERT(NORMAL_GAIN <= Value && Value <= 120, "Invalid Value %f", Value);

		m_Gain = Value;
		m_LinearGain = m_Gain;
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	//[-1, 1]
	void SetAsymmetryLevel(float Value)
	{
		ASSERT(-1 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_AsymmetryLevel = Value;
	}
	float GetAsymmetryLevel(void) const
	{
		return m_AsymmetryLevel;
	}

	void Process(T* Buffer, uint8 Count) override
	{
		m_HighPassFilter.Process(Buffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= Math::GetAsymmetricGain(Buffer[i], m_AsymmetryLevel);

		m_DiodeFilter.Process(Buffer, Count);
	}

private:
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_AsymmetryLevel;
	HighPassResistorCapacitorFilter<T, SampleRate> m_HighPassFilter;
	ClipperDiodeFilter<T, SampleRate, DiodeType> m_DiodeFilter;
};

template<typename T, uint32 SampleRate>
using SoftSignalClipperFilter = SignalClipperFilter<T, SampleRate, ClipperDiodeFilterTypes::Germanium>;

template<typename T, uint32 SampleRate>
using HardSignalClipperFilter = SignalClipperFilter<T, SampleRate, ClipperDiodeFilterTypes::RedLED>;

#endif