#pragma once
#ifndef LED_BASE_H
#define LED_BASE_H

#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Math.h"
#include "DigitalSignalProcessing/IHAL.h"
#include "DigitalSignalProcessing/FixedFunction.h"

class LEDBase
{
public:
	typedef FixedFunction<float(void)> BrightnessFunction;

public:
	LEDBase(IHAL* HAL);

	void SetBrightnessFunction(BrightnessFunction Function)
	{
		m_Function = Function;
	}
	float GetBrightness(void) const
	{
		return Math::Clamp01(m_Function());
	}

	void SetConstantBrightness(cc_ratio_t Value);

	void SetBlinkingBrightness(cc_ratio_t Value, pfloat Rate)
	{
		SetBlinkingBrightness(0, Value, Rate);
	}
	void SetBlinkingBrightness(cc_ratio_t MinValue, cc_ratio_t MaxValue, pfloat Rate);

	void SetFadingBrightness(cc_ratio_t Value, pfloat Rate)
	{
		SetFadingBrightness(0, Value, Rate);
	}
	void SetFadingBrightness(cc_ratio_t MinValue, cc_ratio_t MaxValue, pfloat Rate);

	void Toggle(void)
	{
		SetConstantBrightness(GetBrightness() == 0 ? 1 : 0);
	}

private:
	IHAL *m_HAL;
	BrightnessFunction m_Function;

	float m_DefaultValue;
	float m_DefaultMinValue;
	float m_DefaultMaxValue;
	float m_DefaultRate;
};

#endif