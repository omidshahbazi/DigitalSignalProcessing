#pragma once
#ifndef LED_BASE_H
#define LED_BASE_H

#include "../Debug.h"
#include "../Math.h"
#include "../IHAL.h"
#include <functional>

class LEDBase
{
public:
	typedef std::function<float(void)> BrightnessFunction;

public:
	LEDBase(IHAL *HAL)
		: m_HAL(HAL)
	{
		SetConstantBrighness(0);
	}

	void SetBrightnessFunction(BrightnessFunction &&Function)
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

		SetBrightnessFunction(
			[Value]()
			{
				return Value;
			});
	}

	void SetBlinkingBrighness(float MinValue, float MaxValue, float Rate)
	{
		ASSERT(0 <= MinValue && MinValue <= 1, "Invalid MinValue");
		ASSERT(0 <= MaxValue && MaxValue <= 1, "Invalid MaxValue");
		ASSERT(0 < Rate, "Invalid Rate");

		SetBrightnessFunction(
			[&]()
			{
				return ((int32)(sin(m_HAL->GetTimeSinceStartup() * 4 * Rate) + 1) == 0 ? MinValue : MaxValue);
			});
	}

	void SetFadingBrighness(float MinValue, float MaxValue, float Rate)
	{
		ASSERT(0 <= MinValue && MinValue <= 1, "Invalid MinValue");
		ASSERT(0 <= MaxValue && MaxValue <= 1, "Invalid MaxValue");
		ASSERT(0 < Rate, "Invalid Rate");

		SetBrightnessFunction(
			[&]()
			{
				return Math::Lerp(MinValue, MaxValue, abs(sin(m_HAL->GetTimeSinceStartup() * 4 * Rate)));
			});
	}

	void Toggle(void)
	{
		SetConstantBrighness(GetBrightness() == 0 ? 1 : 0);
	}

private:
	IHAL *m_HAL;
	BrightnessFunction m_Function;
};

#endif