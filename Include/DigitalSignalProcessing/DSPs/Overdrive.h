#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Filters/GainStageFilter.h"
#include "DigitalSignalProcessing/Filters/DiodeClipperFilter.h"
#include "DigitalSignalProcessing/Filters/UpSamplerFilter.h"
#include "DigitalSignalProcessing/Filters/ResistorCapacitorFilter.h"
#include "DigitalSignalProcessing/Filters/EnvelopeFollowerFilter.h"

template <typename T, uint32_t SampleRate, typename Clipper>
class Overdrive : public IDSP<T, SampleRate>
{
public:
	Overdrive(void)
		: m_Drive(0),
		m_Tone(0),
		m_WetRate(0)
	{
		SetDrive(0.5);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
		SetPowerSag(0.5);
		SetWetRate(1);

		m_PreFilter.SetCutoffFrequency(Frequency(720));
		m_EnvelopeFollowerFilter.SetAttackTime(5 ms);
		m_EnvelopeFollowerFilter.SetReleaseTime(100 ms);
		m_GainStage.GetClipper().SetAsymmetry(0.3);
	}

	//[0, 1]
	void SetDrive(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_Drive = Value;

		m_GainStage.SetInputGain((LinearGain)Math::Lerp(1, 50, m_Drive));
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

		m_ToneFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(1 KHz, 9 KHz, m_Tone));
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
	void SetPowerSag(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

		m_PowerSag = Value;
	}
	float GetPowerSag(void) const
	{
		return m_PowerSag;
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

		m_PreFilter.Process(Buffer, Count);

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= 1 + m_EnvelopeFollowerFilter.Process(Buffer[i]) * m_PowerSag;

		T* upBuffer = m_UpSampler.Process(Buffer);
		{
			m_GainStage.Process(upBuffer, m_UpSampler.GetCount());

			m_UpSampler.DownSample(Buffer);
		}

		m_ToneFilter.Process(Buffer, Count);

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] = Math::LinearCrossFadeMix(dryBuffer[i], Buffer[i], m_WetRate);

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= m_LinearGain;
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	HighPassResistorCapacitorFilter<T, SampleRate> m_PreFilter;
	GainStageFilter<T, SampleRate* UP_SAMPLE_FACTOR, Clipper> m_GainStage;
	UpSamplerFilter<T, SampleRate, FrameLength, UP_SAMPLE_FACTOR> m_UpSampler;
	LowPassResistorCapacitorFilter<T, SampleRate> m_ToneFilter;

	float m_Drive;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_LinearGain;
	float m_PowerSag;
	float m_WetRate;
};

template<typename T, uint32_t SampleRate, uint32_t ClipperSampleRate = SampleRate>
using DefaultOverdrive = Overdrive<T, SampleRate, SiliconDiodeClipperFilter<T, ClipperSampleRate>>;

#endif