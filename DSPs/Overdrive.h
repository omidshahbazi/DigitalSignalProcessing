//#pragma once
//#ifndef OVERDRIVE_H
//#define OVERDRIVE_H
//
//#include "IDSP.h"
//#include "../Debug.h"
//#include "../Math.h"
//#include "../Filters/LowPassFilter.h"
//#include "../Filters/HighPassFilter.h"
//#include "../Filters/SignalClipperFilter.h"
//
//template <typename T, uint32 SampleRate>
//class Overdrive : public IDSP<T, SampleRate>
//{
//public:
//	Overdrive(void)
//		: m_Drive(0),
//		  m_Tone(0),
//		  m_WetRate(0)
//	{
//		SetDrive(0.5);
//		SetTone(0.5);
//		SetGain(NORMAL_GAIN);
//		SetWetRate(1);
//	}
//
//	//[0, 1]
//	void SetDrive(float Value)
//	{
//		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);
//
//		m_Drive = Value;
//
//		m_PreGain = (LinearGain)Math::Lerp(10, 100, m_Drive);
//		//m_InvertedPreGain = (LinearGain)(1 / Math::SquareRoot((float)m_PreGain));
//
//		m_PreFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(100.0, 720.0, m_Drive));
//		m_PreFilter.SetCutoffFrequency((Frequency)100);
//	}
//	float GetDrive(void) const
//	{
//		return m_Drive;
//	}
//
//	//[0, 1]
//	void SetTone(float Value)
//	{
//		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);
//
//		m_Tone = Value;
//
//		m_PostFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(1 KHz, 8 KHz, m_Tone));
//	}
//	float GetTone(void) const
//	{
//		return m_Tone;
//	}
//
//	//[-12dB, 12dB]
//	void SetGain(dBGain Value)
//	{
//		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);
//
//		m_Gain = Value;
//
//		m_LinearGain = m_Gain;
//	}
//	dBGain GetGain(void) const
//	{
//		return m_Gain;
//	}
//
//	//[0, 1]
//	void SetWetRate(float Value)
//	{
//		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);
//
//		m_WetRate = Value;
//	}
//	float GetWetRate(void) const
//	{
//		return m_WetRate;
//	}
//
//	void Process(T *Buffer, uint8 Count) override
//	{
//		CLONE_BUFFER(dryBuffer);
//
//		CREATE_STANDARD_UP_SAMPLE_BUFFER(upBuffer);
//		{
//			Math::UpSample(Buffer, Count, upBuffer, upBufferFactor);
//
//			m_PreFilter.Process(upBuffer, upBufferLength);
//
//			for (uint8 i = 0; i < upBufferLength; ++i)
//				upBuffer[i] = m_Clipper.Clip(upBuffer[i] * m_PreGain);
//
//			m_PostFilter.Process(upBuffer, upBufferLength);
//
//			for (uint8 i = 0; i < upBufferLength; ++i)
//				upBuffer[i] *= m_InvertedPreGain;
//
//			Math::DownSample(upBuffer, upBufferLength, Buffer, upBufferFactor);
//		}
//
//		for (uint8 i = 0; i < Count; ++i)
//			Buffer[i] = Math::LinearCrossFadeMix(dryBuffer[i], Buffer[i], m_WetRate);
//
//		for (uint8 i = 0; i < Count; ++i)
//			Buffer[i] *= m_LinearGain;
//
//		for (uint8 i = 0; i < Count; ++i)
//			Buffer[i] = Math::SoftClip(Buffer[i]);
//	}
//
//private:
//	HighPassFilter<T, SampleRate* STANDARD_UP_SAMPLE_FACTOR> m_PreFilter;
//	LowPassFilter<T, SampleRate* STANDARD_UP_SAMPLE_FACTOR> m_PostFilter;
//	SoftClipper<T> m_Clipper;
//
//	float m_Drive;
//	LinearGain m_PreGain;
//	LinearGain m_InvertedPreGain;
//	float m_Tone;
//	dBGain m_Gain;
//	LinearGain m_LinearGain;
//	float m_WetRate;
//};
//
//#endif



#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "../Debug.h"
#include "../Math.h"
#include "../Filters/LowPassFilter.h"
#include "../Filters/HighPassFilter.h"
#include "../Filters/LowShelfFilter.h"
#include "../Filters/HighShelfFilter.h"

template <typename T, uint32 SampleRate>
class Overdrive : public IDSP<T, SampleRate>
{
public:
	Overdrive(void)
		: m_Drive(0),
		m_Tone(0),
		m_WetRate(0)
	{
		m_PostFilter1.SetCutoffFrequency(Frequency(720));
		m_PostFilter2.SetCutoffFrequency(Frequency(3.5 KHz));
		m_PostFilter3.SetCutoffFrequency(Frequency(6 KHz));

		SetDrive(0.5);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
		SetWetRate(1);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Drive = Value;

		m_DriveGain = (LinearGain)Math::Lerp(1.5, 5.0, m_Drive);

		m_PreFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(60.0, 180.0, m_Drive));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Tone = Value;

		m_PostFilter1.SetGain((dBGain)Math::Lerp(4, -6, m_Tone));
		m_PostFilter2.SetGain((dBGain)Math::Lerp(-8, -3, m_Tone));
		m_PostFilter3.SetCutoffFrequency((Frequency)Math::FrequencyLerp(3000.0, 8000.0, m_Tone));
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	//[-12dB, 12dB]
	void SetGain(dBGain Value)
	{
		ASSERT(-12 <= Value && Value <= 12, "Invalid Value %f", Value);

		m_Gain = Value;
		m_LinearGain = m_Gain;
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
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

	void Process(T* Buffer, uint8 Count) override
	{
		CLONE_BUFFER(wetBuffer);

		m_PreFilter.Process(wetBuffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			wetBuffer[i] = Math::SoftClip(wetBuffer[i] * m_DriveGain, 0.3);
		//wetBuffer[i] = tanh(wetBuffer[i] * m_DriveGain);//, 0.3

		m_PostFilter1.Process(wetBuffer, Count);
		m_PostFilter2.Process(wetBuffer, Count);
		m_PostFilter3.Process(wetBuffer, Count);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] = Math::LinearCrossFadeMix(Buffer[i], wetBuffer[i], m_WetRate);

		for (uint8 i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;

		//for (uint8 i = 0; i < Count; ++i)
		//	Buffer[i] = Math::SoftClip(Buffer[i]);
	}

private:
	HighPassFilter<T, SampleRate> m_PreFilter;
	LowShelfFilter<T, SampleRate> m_PostFilter1;
	HighShelfFilter<T, SampleRate> m_PostFilter2;
	LowPassFilter<T, SampleRate> m_PostFilter3;

	float m_Drive;
	LinearGain m_DriveGain;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_WetRate;
};

#endif