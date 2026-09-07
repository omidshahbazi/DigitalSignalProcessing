#pragma once
#ifndef TRIPLE_LED_H
#define TRIPLE_LED_H

#include "Control.h"
#include "LEDBase.h"
#include "../Color.h"

class TripleLED : public ControlBase, public LEDBase
{
public:
	TripleLED(IHAL* HAL, uint8_t RedPin, uint8_t GreenPin, uint8_t BluePin, uint16_t UpdateRate, bool UsePWM = false);

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
	Control m_LEDBlue;
	Color m_Color;
	bool m_UsePWM;
};

#endif