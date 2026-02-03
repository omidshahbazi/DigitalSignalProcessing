#pragma once
#ifndef SINGLE_LED_H
#define SINGLE_LED_H

#include "Control.h"
#include "LEDBase.h"

class SingleLED : public Control, public LEDBase
{
public:
	SingleLED(IHAL *HAL, uint8 Pin, uint16 UpdateRate, bool UsePWM = false)
		: Control(HAL, Pin, (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput), UpdateRate),
		  LEDBase(HAL),
		  m_UsePWM(UsePWM)
	{
	}

protected:
	void Update(void) override
	{
		if (m_UsePWM)
			PWMWrite(LEDBase::GetBrightness());
		else
			DigitalWrite(LEDBase::GetBrightness());
	}

private:
	bool m_UsePWM;
};

#endif