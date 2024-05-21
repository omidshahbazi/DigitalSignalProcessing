#pragma once
#ifndef SINGLE_LED_H
#define SINGLE_LED_H
#pragma GCC push_options
#pragma GCC optimize("Os")

#include "Control.h"
#include "LEDBase.h"

class SingleLED : public Control, public LEDBase
{
public:
	SingleLED(IHAL *HAL, uint8 Pin, uint16 UpdateRate)
		: Control(HAL, Pin, IHAL::PinModes::PWM, UpdateRate),
		  LEDBase(HAL)
	{
	}

protected:
	void Update(void) override
	{
		PWMWrite(LEDBase::GetBrightness());
	}
};

#endif
#pragma GCC pop_options