#pragma once
#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Filters/GainStageFilter.h"
#include "DigitalSignalProcessing/Filters/DiodeClipperFilter.h"
#include "DigitalSignalProcessing/Filters/UpSamplerFilter.h"
#include "DigitalSignalProcessing/Filters/ResistorCapacitorFilter.h"
#include "DigitalSignalProcessing/Filters/EnvelopeFollowerFilter.h"

struct OverdriveSettings
{
public:
	Frequency PreHighPassFrequency = Frequency(400);
	Frequency PostHighPassFrequency = Frequency(20);
	Frequency PostLowPassFrequency = Frequency(4 KHz);

	dBGain MinDrive = dBGain(0);
	dBGain MaxDrive = dBGain(40);

	Frequency MinToneFrequency = Frequency(800);
	Frequency MaxToneFrequency = Frequency(6 KHz);

	cc_ratio_t Asymmetry = 0.2f;

	LinearGain DirectPathGain = LinearGain(0);
	pfloat OutputCompensation = 1;
};

template <typename T, uint32_t SampleRate, typename Clipper>
class Overdrive : public IDSP<T, SampleRate>
{
public:
	Overdrive(const OverdriveSettings& Settings) :
		m_Settings(Settings)
	{
		SetDrive(0.5);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
		SetPowerSag(0);

		m_PreFilter.SetCutoffFrequency(m_Settings.PreHighPassFrequency);
		m_PostFilter1.SetCutoffFrequency(m_Settings.PostHighPassFrequency);
		m_PostFilter2.SetCutoffFrequency(m_Settings.PostLowPassFrequency);
		m_EnvelopeFollowerFilter.SetAttackTime(5 ms);
		m_EnvelopeFollowerFilter.SetReleaseTime(100 ms);
		m_GainStage.GetClipper().SetAsymmetry(m_Settings.Asymmetry);
	}
	Overdrive(const OverdriveSettings&&) = delete;

	void SetDrive(cc_ratio_t Value)
	{
		m_Drive = Value;

		m_GainStage.SetInputGain((LinearGain)Math::ExponentialLerp((float)(LinearGain)m_Settings.MinDrive, (float)(LinearGain)m_Settings.MaxDrive, m_Drive));
	}
	cc_ratio_t GetDrive(void) const
	{
		return m_Drive;
	}

	void SetTone(cc_ratio_t Value)
	{
		m_Tone = Value;

		m_ToneFilter.SetCutoffFrequency((Frequency)Math::FrequencyLerp(m_Settings.MinToneFrequency, m_Settings.MaxToneFrequency, m_Tone));
	}
	float GetTone(void) const
	{
		return m_Tone;
	}

	void SetGain(dBGain_12 Value)
	{
		m_Gain = Value;

		m_OutputGain = m_Gain;
		m_OutputGain = LinearGain(m_OutputGain * m_Settings.OutputCompensation);
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
		CLONE_BUFFER(dryBuffer);

		m_PreFilter.Process(Buffer, Count);

		if (m_PowerSag != 0)
			for (uint8_t i = 0; i < Count; ++i)
				Buffer[i] *= 1 + m_EnvelopeFollowerFilter.Process(Buffer[i]) * m_PowerSag;

		T* upBuffer = m_UpSampler.Process(Buffer);
		{
			m_GainStage.Process(upBuffer, m_UpSampler.GetCount());

			m_UpSampler.DownSample(Buffer);
		}

		if (m_Settings.DirectPathGain != 0)
			for (uint8_t i = 0; i < Count; ++i)
				Buffer[i] += dryBuffer[i] * m_Settings.DirectPathGain;

		m_PostFilter1.Process(Buffer, Count);
		m_PostFilter2.Process(Buffer, Count);

		m_ToneFilter.Process(Buffer, Count);

		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] *= m_OutputGain;
	}

private:
	const OverdriveSettings& m_Settings;

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

template<typename T, uint32_t SampleRate>
using DefaultOverdrive = Overdrive<T, SampleRate, SiliconDiodeClipperFilter<T, SampleRate>>;

#endif