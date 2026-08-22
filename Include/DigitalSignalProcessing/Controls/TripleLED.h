#pragma once
#ifndef TRIPLE_LED_H
#define TRIPLE_LED_H

#include "Control.h"
#include "LEDBase.h"

class TripleLED : public ControlBase, public LEDBase
{
public:
	TripleLED(IHAL *HAL, uint8 RedPin, uint8 GreenPin, uint8 BluePin, uint16 UpdateRate, bool UsePWM = false)
		: ControlBase(HAL, UpdateRate),
		  LEDBase(HAL),
		  m_LEDRed(HAL, RedPin, (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput), UpdateRate),
		  m_LEDGreen(HAL, GreenPin, (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput), UpdateRate),
		  m_LEDBlue(HAL, BluePin, (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput), UpdateRate),
		  m_UsePWM(UsePWM)
	{
		IHAL::PinModes pinMode = (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput);
		SetPinMode(RedPin, pinMode);
		SetPinMode(GreenPin, pinMode);
		SetPinMode(BluePin, pinMode);
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
		float brightness = LEDBase::GetBrightness();

		if (m_UsePWM)
		{
		m_LEDRed.PWMWrite((m_Color.R / 255.0) * brightness);
		m_LEDGreen.PWMWrite((m_Color.G / 255.0) * brightness);
		m_LEDBlue.PWMWrite((m_Color.B / 255.0) * brightness);
		}
		else
		{
			m_LEDRed.DigitalWrite(brightness);
			m_LEDGreen.DigitalWrite(brightness);
			m_LEDBlue.DigitalWrite(brightness);
		}
	}

private:
	Control m_LEDRed;
	Control m_LEDGreen;
	Control m_LEDBlue;
	Color m_Color;
	bool m_UsePWM;
};

#endif