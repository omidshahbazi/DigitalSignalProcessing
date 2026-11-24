#pragma once
#ifndef DISTORTION_H
#define DISTORTION_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Filters/UpSamplerFilter.h"
#include "DigitalSignalProcessing/Filters/ResistorCapacitorFilter.h"
#include "DigitalSignalProcessing/Filters/GainStageFilter.h"
#include "DigitalSignalProcessing/Filters/TransistorClipperFilter.h"
#include "DigitalSignalProcessing/Filters/DiodeClipperFilter.h"

struct DistortionSettings
{
public:
	dBGain BoosterGain = dBGain(20);
	Frequency BoosterHighPassFrequency = Frequency(30);
	cc_ratio_t BoosterAsymmetry = 0.1f;

	Frequency MinPreHighPassFrequency = Frequency(30);
	Frequency MaxPreHighPassFrequency = Frequency(300);

	dBGain MinDrive = dBGain(0);
	dBGain MaxDrive = dBGain(30);

	LinearGain DirectPathGain = LinearGain(0.5);

	Frequency PostLowPassFrequency = Frequency(8 KHz);

	Frequency ToneLowPassFrequency = Frequency(500);
	Frequency ToneHighPassFrequency = Frequency(1.5 KHz);

	pfloat OutputCompensation = 1;

	Frequency OutputHighPassFrequency = Frequency(5);
};

template <typename T, uint32_t SampleRate, typename BoosterClipper, typename DiodeClipper>
class Distortion : public IDSP<T, SampleRate>
{
public:
	Distortion(const DistortionSettings& Settings) :
		m_Settings(Settings)
	{
		m_InputFilter.SetCutoffFrequency(m_Settings.BoosterHighPassFrequency);
		m_PostLowPass.SetCutoffFrequency(m_Settings.PostLowPassFrequency);
		m_ToneLowPass.SetCutoffFrequency(m_Settings.ToneLowPassFrequency);
		m_ToneHighPass.SetCutoffFrequency(m_Settings.ToneHighPassFrequency);
		m_OutputFilter.SetCutoffFrequency(m_Settings.OutputHighPassFrequency);

		m_Booster.GetClipper().SetAsymmetry(m_Settings.BoosterAsymmetry);
		m_Booster.SetInputGain(m_Settings.BoosterGain);

		SetDrive(0.5);
		SetAsymmetryLevel(0);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
	}
	Distortion(const DistortionSettings&&) = delete;

	void SetDrive(cc_ratio_t Value)
	{
		m_Drive = Value;

		m_DriveGain = (LinearGain)dBGain::Interpolate(m_Settings.MinDrive, m_Settings.MaxDrive, m_Drive);

		m_PreFilter.SetCutoffFrequency(Frequency::Interpolate(m_Settings.MinPreHighPassFrequency, m_Settings.MaxPreHighPassFrequency, m_Drive));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	void SetAsymmetryLevel(cc_ratio_t Value)
	{
		m_Diodes.SetAsymmetry(Value);
	}
	float GetAsymmetryLevel(void) const
	{
		return m_Diodes.GetAsymmetry();
	}

	void SetTone(cc_ratio_t Value)
	{
		m_Tone = Value;
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

	void Process(T* Buffer, uint8_t Count) override
	{
		m_InputFilter.Process(Buffer, Count);

		T* upBuffer = m_UpSampler.Process(Buffer);
		{
			const uint8_t upCount = m_UpSampler.GetCount();

			m_Booster.Process(upBuffer, upCount);

			CLONE_STANDARD_UP_SAMPLE_BUFFER(m_UpSampler, directBuffer);

			m_PreFilter.Process(upBuffer, upCount);

			for (uint8_t i = 0; i < upCount; ++i)
				upBuffer[i] = directBuffer[i] * m_Settings.DirectPathGain + upBuffer[i] * m_DriveGain;

			m_Diodes.Process(upBuffer, upCount);

			m_UpSampler.DownSample(Buffer);
		}

		m_PostLowPass.Process(Buffer, Count);

		CLONE_BUFFER(toneBuffer);

		m_ToneLowPass.Process(Buffer, Count);
		m_ToneHighPass.Process(toneBuffer, Count);

		const float LowWeight = Math::Max(1.0f - m_Tone, 0.25F);
		for (uint8_t i = 0; i < Count; ++i)
			Buffer[i] = (Buffer[i] * LowWeight + toneBuffer[i] * m_Tone) * m_OutputGain;

		m_OutputFilter.Process(Buffer, Count);
	}

private:
	const DistortionSettings& m_Settings;

	HighPassResistorCapacitorFilter<T, SampleRate> m_InputFilter;
	UpSamplerFilter<T, SampleRate, FrameLength, UP_SAMPLE_FACTOR, true> m_UpSampler;
	GainStageFilter<T, UpSampleSampleRate, BoosterClipper> m_Booster;
	HighPassResistorCapacitorFilter<T, UpSampleSampleRate> m_PreFilter;
	DiodeClipper m_Diodes;
	LowPassResistorCapacitorFilter<T, SampleRate> m_PostLowPass;
	LowPassResistorCapacitorFilter<T, SampleRate> m_ToneLowPass;
	HighPassResistorCapacitorFilter<T, SampleRate> m_ToneHighPass;
	HighPassResistorCapacitorFilter<T, SampleRate> m_OutputFilter;

	float m_Drive;
	LinearGain m_DriveGain;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_OutputGain;
};

template<typename T, uint32_t SampleRate>
using DefaultDistortion = Distortion<T, SampleRate, TransistorClipperFilter<T, SampleRate>, SiliconDiodeClipperFilter<T, SampleRate>>;

#endif