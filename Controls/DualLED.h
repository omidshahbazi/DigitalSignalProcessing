#pragma once
#ifndef DUAL_LED_H
#define DUAL_LED_H

#include "Control.h"
#include "LEDBase.h"

class DualLED : public ControlBase, public LEDBase
{
public:
	struct Color
	{
	public:
		float Red;
		float Green;
	};

public:
	DualLED(IHAL *HAL, uint8 RedPin, uint8 GreenPin, uint16 UpdateRate)
		: ControlBase(HAL, UpdateRate),
		  m_LEDRed(HAL, RedPin, IHAL::PinModes::PWM, UpdateRate),
		  m_LEDGreen(HAL, GreenPin, IHAL::PinModes::PWM, UpdateRate)
	{
		SetPinMode(RedPin, IHAL::PinModes::PWM);
		SetPinMode(GreenPin, IHAL::PinModes::PWM);
	}

	void SetColor(const Color &Value)
	{
		m_Color = Value;

		m_Color.Red = Math::Clamp01(m_Color.Red);
		m_Color.Green = Math::Clamp01(m_Color.Green);
	}
	const Color &GetColor(void) const
	{
		return m_Color;
	}

protected:
	void Update(void) override
	{
		float multiplier = LEDBase::GetBrightness();

		m_LEDRed.PWMWrite(m_Color.Red * multiplier);
		m_LEDGreen.PWMWrite(m_Color.Green * multiplier);
	}

private:
	Control m_LEDRed;
	Control m_LEDGreen;
	Color m_Color;
};

#define DUAL_LED_RED \
	{                \
		1, 0         \
	}
#define DUAL_LED_GREEN \
	{                  \
		0, 1           \
	}
#define DUAL_LED_YELLOW \
	{                   \
		1, 1            \
	}

#endif