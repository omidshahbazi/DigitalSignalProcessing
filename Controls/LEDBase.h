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
	LEDBase(IHAL *HAL)
		: m_HAL(HAL)
	{
		SetConstantBrighness(0);
	}

	void SetBrightnessFunction(BrightnessFunction Function)
	{
		ASSERT(Function != nullptr, "Invalid Function");

		m_Function = Function;
	}
	float GetBrightness(void) const
	{
		return Math::Clamp01(m_Function());
	}

	void SetConstantBrighness(float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		SetBrightnessFunction({this,
							   [](void *Context)
							   {
								   return static_cast<LEDBase *>(Context)->m_DefaultValue;
							   }});
	}

	void SetBlinkingBrighness(float Value, float Rate)
	{
		SetBlinkingBrighness(0, Value, Rate);
	}
	void SetBlinkingBrighness(float MinValue, float MaxValue, float Rate)
	{
		ASSERT(0 <= MinValue && MinValue <= 1, "Invalid MinValue");
		ASSERT(0 <= MaxValue && MaxValue <= 1, "Invalid MaxValue");
		ASSERT(0 < Rate, "Invalid Rate");

		SetBrightnessFunction({this,
							   [](void *Context)
							   {
								   LEDBase *led = static_cast<LEDBase *>(Context);

								   float value = Math::Moderate(led->m_HAL->GetTimeSinceStartup() * 2 * led->m_DefaultRate, 2);

								   return ((int32)value == 0 ? led->m_DefaultMinValue : led->m_DefaultMaxValue);
							   }});
	}

	void SetFadingBrighness(float Value, float Rate)
	{
		SetFadingBrighness(0, Value, Rate);
	}
	void SetFadingBrighness(float MinValue, float MaxValue, float Rate)
	{
		ASSERT(0 <= MinValue && MinValue <= 1, "Invalid MinValue");
		ASSERT(0 <= MaxValue && MaxValue <= 1, "Invalid MaxValue");
		ASSERT(0 < Rate, "Invalid Rate");

		SetBrightnessFunction({this,
							   [](void *Context)
							   {
								   LEDBase *led = static_cast<LEDBase *>(Context);

								   float value = Math::Moderate(led->m_HAL->GetTimeSinceStartup() * 2 * led->m_DefaultRate, 2);
								   if (value > 1)
									   value = 1 - (value - 1);

								   return Math::Lerp(led->m_DefaultMinValue, led->m_DefaultMaxValue, value);
							   }});
	}

	void Toggle(void)
	{
		SetConstantBrighness(GetBrightness() == 0 ? 1 : 0);
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