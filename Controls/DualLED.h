#pragma once
#ifndef DUAL_LED_H
#define DUAL_LED_H

#include "Control.h"
#include "LEDBase.h"

class DualLED : public ControlBase, public LEDBase
{
public:
	DualLED(IHAL *HAL, uint8 RedPin, uint8 GreenPin, uint16 UpdateRate)
		: ControlBase(HAL, UpdateRate),
		  LEDBase(HAL),
		  m_LEDRed(HAL, RedPin, IHAL::PinModes::PWM, UpdateRate),
		  m_LEDGreen(HAL, GreenPin, IHAL::PinModes::PWM, UpdateRate)
	{
		SetPinMode(RedPin, IHAL::PinModes::PWM);
		SetPinMode(GreenPin, IHAL::PinModes::PWM);
	}

	void SetColor(const Color &Value)
	{
		m_Color = Value;

		m_Color.R = Math::Clamp(m_Color.R, 0, 255);
		m_Color.G = Math::Clamp(m_Color.G, 0, 255);
	}
	const Color &GetColor(void) const
	{
		return m_Color;
	}

protected:
	void Update(void) override
	{
		float multiplier = LEDBase::GetBrightness();

		m_LEDRed.PWMWrite((m_Color.R / 255.0) * multiplier);
		m_LEDGreen.PWMWrite((m_Color.G / 255.0) * multiplier);
	}

private:
	Control m_LEDRed;
	Control m_LEDGreen;
	Color m_Color;
};

#endif