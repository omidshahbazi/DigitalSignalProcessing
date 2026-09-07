#pragma once
#ifndef ROTARY_BUTTON_H
#define ROTARY_BUTTON_H

#include "Rotary.h"
#include "Button.h"

class RotaryButton : public Button
{
public:
	typedef Rotary::RotatedEventHandler RotatedEventHandler;

public:
	RotaryButton(IHAL* HAL, uint8_t APin, uint8_t BPin, uint8_t ButtonPin);

	void SetOnRotatedListener(RotatedEventHandler Listener)
	{
		m_Rotary.SetOnRotatedListener(Listener);
	}

protected:
	virtual void Update(void) override;

private:
	Rotary m_Rotary;
};

#endif