#pragma once
#ifndef TRANSISTOR_CLIPPER_FILTER_H
#define TRANSISTOR_CLIPPER_FILTER_H

#include "ClipperFilter.h"

template<typename T, uint32_t SampleRate>
class TransistorClipperFilter : public ClipperFilter<T, SampleRate>
{
private:
	typedef ClipperFilter<T, SampleRate> Base;

public:
	TransistorClipperFilter(void)
		: m_SquareCoeff(0),
		m_CubeCoeff(0)
	{
		SetInputGain(NORMAL_GAIN);
	}

	void SetInputGain(LinearGain Value) override
	{
		float ratio = Math::Map((float)Value, 1.0f, 35.0f, 0.0f, 1.0f);

		m_SquareCoeff = Math::Lerp(0.1f, 0.5f, ratio);
		m_CubeCoeff = Math::Lerp(0.05f, 0.3f, ratio);
	}

	void Process(T* Buffer, uint8_t Count) override
	{
		for (uint8_t i = 0; i < Count; ++i)
		{
			T x = Buffer[i];

			T absX = Math::Absolute(x);

			float asymmetricFactor = (x >= 0) ? 1 : (1 - (Base::GetAsymmetry() * 0.5));

			T squared = x * absX * asymmetricFactor;
			T cubed = x * x * x;

			T processed = x - (m_SquareCoeff * squared * asymmetricFactor) - (m_CubeCoeff * cubed);

			Buffer[i] = Math::TanH(processed);
		}
	}

	FILTER_FORWARD_DECLARATION()

private:
	float m_SquareCoeff;
	float m_CubeCoeff;
};

#endif