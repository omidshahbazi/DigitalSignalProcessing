#pragma once
#ifndef ROTARY_BUTTON_H
#define ROTARY_BUTTON_H

#include "Rotary.h"
#include "Button.h"

class RotaryButton : public Button
{
public:
	RotaryButton(IHAL *HAL, uint8 LeftPin, uint8 RightPin, uint8 ButtonPin, uint16 UpdateRate)
		: Button(HAL, ButtonPin, UpdateRate),
		  m_Rotary(HAL, LeftPin, RightPin, UpdateRate)
	{
	}

	void SetOnRotatedListener(Rotary::RotatedEventHandler Listener)
	{
		m_Rotary.SetOnRotatedListener(Listener);
	}

protected:
	virtual void Update(void) override
	{
		m_Rotary.Update();

		Button::Update();
	}

private:
	Rotary m_Rotary;
};

#endif