#pragma once
#ifndef METAL_NOISE_FILTER_H
#define METAL_NOISE_FILTER_H

#include "Filter.h"
#include "../Debug.h"

template <typename T, uint32 SampleRate>
class MetalNoiseFilter : public Filter<T, SampleRate>
{
public:
	MetalNoiseFilter(void)
		: m_Phase{}
	{
		// TR-808 based inharmonic frequencies for metallic texture
		const float Frequencies[6] = {245, 306, 384, 522, 650, 800};

		for (uint8 i = 0; i < 6; ++i)
		{
			m_PhaseIncrement[i] = Frequencies[i] / SampleRate;
			m_Phase[i] = 0.0f;
		}
	}

	void Process(T *Buffer, uint8 Count) override
	{
		for (uint8 j = 0; j < Count; ++j)
		{
			float combinedSource = 0.0f;

			for (uint8 i = 0; i < 6; ++i)
			{
				m_Phase[i] += m_PhaseIncrement[i];
				if (m_Phase[i] >= 1.0f)
					m_Phase[i] -= 1.0f;

				combinedSource += (m_Phase[i] < 0.5f) ? 1.0f : -1.0f;
			}

			Buffer[j] = (T)(combinedSource * 0.166f);
		}
	}

	using Filter<T, SampleRate>::Process;

private:
	float m_PhaseIncrement[6];
	float m_Phase[6];
};

#endif