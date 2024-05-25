#pragma once
#ifndef TRIPLE_LED_H
#define TRIPLE_LED_H

#include "Control.h"
#include "LEDBase.h"

class TripleLED : public ControlBase, public LEDBase
{
public:
	TripleLED(IHAL *HAL, uint8 RedPin, uint8 GreenPin, uint8 BluePin, uint16 UpdateRate)
		: ControlBase(HAL, UpdateRate),
		  LEDBase(HAL),
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

		m_Color.R = Math::Clamp(m_Color.R, 0, 255);
		m_Color.G = Math::Clamp(m_Color.G, 0, 255);
		m_Color.B = Math::Clamp(m_Color.B, 0, 255);
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
		m_LEDBlue.PWMWrite((m_Color.B / 255.0) * multiplier);
	}

private:
	Control m_LEDRed;
	Control m_LEDGreen;
	Control m_LEDBlue;
	Color m_Color;
};

#endif