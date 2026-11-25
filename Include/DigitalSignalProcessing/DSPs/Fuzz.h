#pragma once
#ifndef FUZZ_H
#define FUZZ_H

#include "IDSP.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/Filters/UpSamplerFilter.h"
#include "DigitalSignalProcessing/Filters/ResistorCapacitorFilter.h"
#include "DigitalSignalProcessing/Filters/GainStageFilter.h"
#include "DigitalSignalProcessing/Filters/TransistorClipperFilter.h"

struct FuzzSettings
{
public:
	Frequency InputHighPassFrequency = Frequency(10);

	cc_ratio_t InputBias = 0;

	dBGain MinDrive = dBGain(20);
	dBGain MaxDrive = dBGain(50);

	cc_ratio_t ClipperAsymmetry = 0.3f;

	Frequency PostLowPassFrequency = Frequency(6 KHz);

	Frequency ToneLowPassFrequency = Frequency(20 KHz);
	Frequency ToneHighPassFrequency = Frequency(20 KHz);

	pfloat OutputCompensation = 1;

	Frequency OutputHighPassFrequency = Frequency(5);
};

template <typename T, uint32_t SampleRate, typename Clipper>
class Fuzz : public IDSP<T, SampleRate>
{
public:
	Fuzz(const FuzzSettings& Settings) :
		m_Settings(Settings)
	{
		m_InputFilter.SetCutoffFrequency(m_Settings.InputHighPassFrequency);
		m_GainStage.SetInputBias(m_Settings.InputBias);
		m_PostLowPass.SetCutoffFrequency(m_Settings.PostLowPassFrequency);
		m_ToneLowPass.SetCutoffFrequency(m_Settings.ToneLowPassFrequency);
		m_ToneHighPass.SetCutoffFrequency(m_Settings.ToneHighPassFrequency);
		m_OutputFilter.SetCutoffFrequency(m_Settings.OutputHighPassFrequency);

		SetDrive(0.5);
		SetAsymmetryLevel(m_Settings.ClipperAsymmetry);
		SetTone(0.5);
		SetGain(NORMAL_GAIN);
	}
	Fuzz(const FuzzSettings&&) = delete;

	void SetDrive(cc_ratio_t Value)
	{
		m_Drive = Value;

		m_GainStage.SetInputGain(dBGain::Interpolate(m_Settings.MinDrive, m_Settings.MaxDrive, m_Drive));
	}
	float GetDrive(void) const
	{
		return m_Drive;
	}

	void SetAsymmetryLevel(cc_ratio_t Value)
	{
		m_GainStage.GetClipper().SetAsymmetry(Value);
	}
	float GetAsymmetryLevel(void) const
	{
		return m_GainStage.GetClipper().GetAsymmetry();
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
			m_GainStage.Process(upBuffer, m_UpSampler.GetCount());

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
	const FuzzSettings& m_Settings;

	HighPassResistorCapacitorFilter<T, SampleRate> m_InputFilter;
	UpSamplerFilter<T, SampleRate, FrameLength, UP_SAMPLE_FACTOR, true> m_UpSampler;
	GainStageFilter<T, UpSampleSampleRate, Clipper> m_GainStage;
	LowPassResistorCapacitorFilter<T, SampleRate> m_PostLowPass;
	LowPassResistorCapacitorFilter<T, SampleRate> m_ToneLowPass;
	HighPassResistorCapacitorFilter<T, SampleRate> m_ToneHighPass;
	HighPassResistorCapacitorFilter<T, SampleRate> m_OutputFilter;

	float m_Drive;
	float m_Tone;
	dBGain m_Gain;
	LinearGain m_OutputGain;
};

template<typename T, uint32_t SampleRate>
using DefaultFuzz = Fuzz<T, SampleRate, TransistorClipperFilter<T, SampleRate>>;

#endif