#pragma once
#ifndef TRIPLE_LED_H
#define TRIPLE_LED_H

#include "Control.h"
#include "LEDBase.h"
#include "../Color.h"

class TripleLED : public ControlBase, public LEDBase
{
public:
	TripleLED(IHAL* HAL, uint8 RedPin, uint8 GreenPin, uint8 BluePin, uint16 UpdateRate, bool UsePWM = false);

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