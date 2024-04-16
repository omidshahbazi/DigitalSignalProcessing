#pragma once
#ifndef LED_BASE_H
#define LED_BASE_H

#include "../Time.h"
#include "../Debug.h"
#include "../Math.h"
#include <functional>

class LEDBase
{
public:
	typedef std::function<float(void)> BrightnessFunction;

public:
	LEDBase(void)
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

	void SetBlinkingBrighness(float Value, float Rate)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");
		ASSERT(0 < Rate, "Invalid Rate");

		SetBrightnessFunction(
			[Value, Rate]()
			{
				return (int32)(sin(Time::GetNow() * 4 * Rate) + 1) * Value;
			});
	}

	void Toggle(void)
	{
		SetConstantBrighness(GetBrightness() == 0 ? 1 : 0);
	}

private:
	BrightnessFunction m_Function;
};

#endif