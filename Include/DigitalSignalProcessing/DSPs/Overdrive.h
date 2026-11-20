#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Filters/GainStageFilter.h"
#include "DigitalSignalProcessing/Filters/DiodeClipperFilter.h"
#include "DigitalSignalProcessing/Filters/UpSamplerFilter.h"
#include "DigitalSignalProcessing/Filters/ResistorCapacitorFilter.h"
#include "DigitalSignalProcessing/Filters/EnvelopeFollowerFilter.h"

//struct OverdriveSettings
//{
//public:
//	Frequency PreHighPassFrequency = Frequency(720);
//	Frequency PostHighPassFrequency = Frequency(20);
//	Frequency PostLowPassFrequency = Frequency(2.4 KHz);
//
//	Frequency MinToneFrequency = Frequency(1 KHz);
//	Frequency MaxToneFrequency = Frequency(5 KHz);
//
//	dBGain MaxDriveGain = dBGain(60);
//
//	//Linear gain applied together with the user gain. 1 = unchanged.
//	LinearGain OutputCompensation = 1;
//
//	cc_ratio_t Asymmetry = 0.3f;
//	cc_ratio_t PowerSag = 0;
//};

template <typename T, uint32_t SampleRate, typename Clipper>
class Overdrive : public IDSP<T, SampleRate>
{
public:
	Overdrive(void)
	{
		SetDrive(0.5);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
		SetPowerSag(0);

		m_PreFilter.SetCutoffFrequency(Frequency(720));
		m_PostFilter1.SetCutoffFrequency(Frequency(20));
		m_PostFilter2.SetCutoffFrequency(Frequency(2.4 KHz));
		m_EnvelopeFollowerFilter.SetAttackTime(5 ms);
		m_EnvelopeFollowerFilter.SetReleaseTime(100 ms);
		m_GainStage.GetClipper().SetAsymmetry(0.3);
	}

	void SetDrive(cc_ratio_t Value)
	{
		m_Drive = Value;

		static const float Max = (LinearGain)dBGain(60);

		m_GainStage.SetInputGain((LinearGain)Math::ExponentialLerp(1.0, Max, m_Drive));
	}
	cc_ratio_t GetDrive(void) const
	{
		return m_Drive;
	}

	void SetTone(cc_ratio_t Value)
	{
		m_Tone = Value;

		m_ToneFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(1 KHz, 5 KHz, m_Tone));
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	void SetGain(dBGain_12 Value)
	{
		m_Gain = Value;

		m_OutputGain = m_Gain;
	}
	dBGain GetGain(void) const
	{
		return m_Gain;
	}

	void SetPowerSag(cc_ratio_t Value)
	{
		m_PowerSag = Value;
	}
	float GetPowerSag(void) const
	{
		return m_PowerSag;
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		m_PreFilter.Process(Buffer, Count);

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= 1 + m_EnvelopeFollowerFilter.Process(Buffer[i]) * m_PowerSag;

		T* upBuffer = m_UpSampler.Process(Buffer);
		{
			m_GainStage.Process(upBuffer, m_UpSampler.GetCount());

			m_UpSampler.DownSample(Buffer);
		}

		m_PostFilter1.Process(Buffer, Count);
		m_PostFilter2.Process(Buffer, Count);

		m_ToneFilter.Process(Buffer, Count);

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= m_OutputGain;
	}

private:
	EnvelopeFollowerFilter<T, SampleRate> m_EnvelopeFollowerFilter;
	HighPassResistorCapacitorFilter<T, SampleRate> m_PreFilter;
	UpSamplerFilter<T, SampleRate, FrameLength, UP_SAMPLE_FACTOR, true> m_UpSampler;
	GainStageFilter<T, SampleRate* UP_SAMPLE_FACTOR, Clipper> m_GainStage;
	HighPassResistorCapacitorFilter<T, SampleRate> m_PostFilter1;
	LowPassResistorCapacitorFilter<T, SampleRate> m_PostFilter2;
	LowPassResistorCapacitorFilter<T, SampleRate> m_ToneFilter;

	float m_PowerSag;
	float m_Drive;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_OutputGain;
};

template<typename T, uint32_t SampleRate, uint32_t ClipperSampleRate = SampleRate>
using DefaultOverdrive = Overdrive<T, SampleRate, SiliconDiodeClipperFilter<T, ClipperSampleRate>>;

#endif