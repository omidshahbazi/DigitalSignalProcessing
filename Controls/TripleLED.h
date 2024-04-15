#pragma once
#ifndef TRIPLE_LED_H
#define TRIPLE_LED_H

#include "Control.h"
#include "LEDBase.h"

class TripleLED : public ControlBase, public LEDBase
{
public:
	struct Color
	{
	public:
		float Red;
		float Green;
		float Blue;
	};

public:
	TripleLED(IHAL *HAL, uint8 RedPin, uint8 GreenPin, uint8 BluePin, uint16 UpdateRate)
		: ControlBase(HAL, UpdateRate),
		  m_LEDRed(HAL, RedPin, IHAL::PinModes::PWM, UpdateRate),
		  m_LEDGreen(HAL, GreenPin, IHAL::PinModes::PWM, UpdateRate),
		  m_LEDBlue(HAL, BluePin, IHAL::PinModes::PWM, UpdateRate)
	{
		SetPinMode(RedPin, IHAL::PinModes::PWM);
		SetPinMode(GreenPin, IHAL::PinModes::PWM);
		SetPinMode(BluePin, IHAL::PinModes::PWM);
	}

	void SetColor(const Color &Value)
	{
		m_Color = Value;

		m_Color.Red = Math::Clamp01(m_Color.Red);
		m_Color.Green = Math::Clamp01(m_Color.Green);
		m_Color.Blue = Math::Clamp01(m_Color.Blue);
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
		m_LEDBlue.PWMWrite(m_Color.Blue * multiplier);
	}

private:
	Control m_LEDRed;
	Control m_LEDGreen;
	Control m_LEDBlue;
	Color m_Color;
};

#define TRIPLE_LED_RED \
	{                  \
		1, 0, 0        \
	}
#define TRIPLE_LED_GREEN \
	{                    \
		0, 1, 0          \
	}
#define TRIPLE_LED_BLUE \
	{                   \
		0, 0, 1         \
	}
#define TRIPLE_LED_CYAN \
	{                   \
		0, 1, 1         \
	}
#define TRIPLE_LED_YELLOW \
	{                     \
		1, 1, 0           \
	}
#define TRIPLE_LED_MAGENTA \
	{                      \
		1, 0, 1            \
	}
#define TRIPLE_LED_WHITE \
	{                    \
		1, 1, 1          \
	}

#endif