#pragma once
#ifndef LED_BASE_H
#define LED_BASE_H

#include "../Debug.h"
#include "../Math.h"
#include "../IHAL.h"
#include "../ContextCallback.h"

class LEDBase
{
public:
	typedef ContextCallback<float> BrightnessFunction;

public:
	LEDBase(IHAL* HAL);

	void SetBrightnessFunction(BrightnessFunction Function)
	{
		ASSERT(Function != nullptr, "Invalid Function");

		m_Function = Function;
	}
	float GetBrightness(void) const
	{
		return Math::Clamp01(m_Function());
	}

	void SetConstantBrightness(float Value);

	void SetBlinkingBrightness(float Value, float Rate)
	{
		SetBlinkingBrightness(0, Value, Rate);
	}
	void SetBlinkingBrightness(float MinValue, float MaxValue, float Rate);

	void SetFadingBrightness(float Value, float Rate)
	{
		SetFadingBrightness(0, Value, Rate);
	}
	void SetFadingBrightness(float MinValue, float MaxValue, float Rate);

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