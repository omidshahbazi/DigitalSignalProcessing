#pragma once
#ifndef DISTORTION_H
#define DISTORTION_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Filters/GainStageFilter.h"
#include "DigitalSignalProcessing/Filters/SimpleClipperFilter.h"
#include "DigitalSignalProcessing/Filters/UpSamplerFilter.h"
#include "DigitalSignalProcessing/Filters/LowPassFilter.h"

template <typename T, uint32_t SampleRate, typename Clipper>
class Distortion : public IDSP<T, SampleRate>
{
public:
	Distortion(void)
		: m_Drive(0),
		m_Tone(0),
		m_WetRate(0)
	{
		SetDrive(0.5);
		SetAsymmetryLevel(0.9);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
		SetWetRate(1);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Drive = Value;

		m_GainStage.SetInputGain((LinearGain)Math::Lerp(5, 100, m_Drive));
		m_InvertedPreGain = (LinearGain)(1 / Math::SquareRoot((float)m_GainStage.GetInputGain()));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	//[0, 1]
	void SetAsymmetryLevel(float Value)
	{
		m_GainStage.GetClipper().SetAsymmetry(Value);
	}
	float GetAsymmetryLevel(void) const
	{
		return m_GainStage.GetClipper().GetAsymmetry();
	}

	//[0, 1]
	void SetTone(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Tone = Value;

		m_ToneFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(800, 3 KHz, m_Tone));
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

	void Process(T* Buffer, uint8_t Count) override
	{
		CLONE_BUFFER(dryBuffer);

		T* upBuffer = m_UpSampler.Process(Buffer);
		{
			m_GainStage.Process(upBuffer, m_UpSampler.GetCount());

			m_UpSampler.DownSample(Buffer);
		}

		//for (uint8_t i = 0; i < Count; ++i)
		//	Buffer[i] *= m_InvertedPreGain;

		m_ToneFilter.Process(Buffer, Count);

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] = Math::LinearCrossFadeMix(dryBuffer[i], Buffer[i], m_WetRate);
	}

private:
	UpSamplerFilter<T, SampleRate, FrameLength, UP_SAMPLE_FACTOR, true> m_UpSampler;
	GainStageFilter<T, SampleRate* UP_SAMPLE_FACTOR, Clipper> m_GainStage;
	LowPassFilter<T, SampleRate> m_ToneFilter;

	float m_Drive;
	LinearGain m_InvertedPreGain;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_WetRate;
};

template<typename T, uint32_t SampleRate, uint32_t ClipperSampleRate = SampleRate>
using DefaultDistortion = Distortion<T, SampleRate, PolynomialSimpleClipperFilter<T, ClipperSampleRate>>;

#endif