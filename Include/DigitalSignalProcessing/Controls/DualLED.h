#pragma once
#ifndef DUAL_LED_H
#define DUAL_LED_H

#include "Control.h"
#include "LEDBase.h"
#include "../Color.h"

class DualLED : public ControlBase, public LEDBase
{
public:
	DualLED(IHAL* HAL, uint8_t RedPin, uint8_t GreenPin, uint16_t UpdateRate, bool UsePWM = false);

	void SetColor(const Color& Value);
	const Color &GetColor(void) const
	{
		return m_Color;
	}

protected:
	void Update(void) override;

private:
	Control m_LEDRed;
	Control m_LEDGreen;
	Color m_Color;
	bool m_UsePWM;
};

#endif