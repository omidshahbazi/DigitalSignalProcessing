#pragma once
#ifndef SINGLE_LED_H
#define SINGLE_LED_H

#include "Control.h"
#include "LEDBase.h"

class SingleLED : public Control, public LEDBase
{
public:
	SingleLED(IHAL* HAL, uint8_t Pin, uint16_t UpdateRate, bool UsePWM = false);

protected:
	void Update(void) override;

private:
	bool m_UsePWM;
};

#endif