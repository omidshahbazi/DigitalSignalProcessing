#pragma once
#ifndef UP_SAMPLER_FILTER_H
#define UP_SAMPLER_FILTER_H

#include "Filter.h"
#include "LowPassFilter.h"
#include "../Math.h"

template <typename T, uint32 SampleRate, uint32 FrameLength, uint8 Ratio, bool ZeroStuffing = true>
class UpSamplerFilter : public Filter<T, SampleRate>
{
public:
	static constexpr uint16 BufferSize = FrameLength * Ratio;

public:
	void Process(T* Buffer, uint8 Count) override
	{
		Math::UpSample<T, ZeroStuffing>(Buffer, Count, m_Samples, Ratio);
	}

	T* Process(T* Buffer)
	{
		Process(Buffer, FrameLength);

		return m_Samples;
	}

	void Makeup(void)
	{
		Math::UpSampleMakeup(m_Samples, BufferSize);
	}

	void DownSample(T* Buffer)
	{
		Math::DownSample<T, ZeroStuffing>(m_Samples, BufferSize, Buffer, Ratio);
	}

	T* GetBuffer(void)
	{
		return m_Samples;
	}

	const T* GetBuffer(void) const
	{
		return m_Samples;
	}

	constexpr uint16 GetCount(void) const
	{
		return BufferSize;
	}

private:
	T m_Samples[BufferSize];
};

template <typename T, uint32 SampleRate, uint32 FrameLength, uint8 Ratio>
class UpSamplerFilter<T, SampleRate, FrameLength, Ratio, true> : public Filter<T, SampleRate>
{
public:
	static constexpr uint16 BufferSize = FrameLength * Ratio;

public:
	UpSamplerFilter(void)
	{
		m_UpSampleFilter.SetCutoffFrequency(MAX_FREQUENCY);
		m_DownSampleFilter.SetCutoffFrequency(MAX_FREQUENCY);
	}

	void Process(T* Buffer, uint8 Count) override
	{
		Math::UpSample<T, true>(Buffer, Count, m_Samples, Ratio);

		m_UpSampleFilter.Process(m_Samples, BufferSize);
	}

	T* Process(T* Buffer)
	{
		Process(Buffer, FrameLength);

		return m_Samples;
	}

	void Makeup(void)
	{
		Math::UpSampleMakeup(m_Samples, BufferSize);
	}

	void DownSample(T* Buffer)
	{
		m_DownSampleFilter.Process(m_Samples, BufferSize);

		Math::DownSample<T, true>(m_Samples, BufferSize, Buffer, Ratio);
	}

	T* GetBuffer(void)
	{
		return m_Samples;
	}

	const T* GetBuffer(void) const
	{
		return m_Samples;
	}

	constexpr uint16 GetCount(void) const
	{
		return BufferSize;
	}

private:
	T m_Samples[BufferSize];
	LowPassFilter<T, SampleRate* Ratio, 2, BiquadFilterDesigns::Butterworth> m_UpSampleFilter;
	LowPassFilter<T, SampleRate* Ratio, 2, BiquadFilterDesigns::Butterworth> m_DownSampleFilter;
};
#endif