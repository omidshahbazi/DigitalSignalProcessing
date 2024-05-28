#pragma once
#ifndef ROTARY_H
#define ROTARY_H

#include "Control.h"

class RotaryButton;

class Rotary : public ControlBase
{
	friend class RotaryButton;

public:
	typedef ContextCallback<void, int8> RotatedEventHandler;

public:
	Rotary(IHAL *HAL, uint8 LeftPin, uint8 RightPin)
		: ControlBase(HAL, 1000),
		  m_LeftControl(HAL, LeftPin, IHAL::PinModes::DigitalInput, 0),
		  m_RightControl(HAL, RightPin, IHAL::PinModes::DigitalInput, 0)
	{
		ASSERT(HAL->IsADigitalPin(LeftPin), "Pin %i is not an digital pin", LeftPin);
		ASSERT(HAL->IsADigitalPin(RightPin), "Pin %i is not an digital pin", RightPin);
	}

	void SetOnRotatedListener(RotatedEventHandler Listener)
	{
		m_OnRotated = Listener;
	}

protected:
	virtual void Update(void) override
	{
		uint8 leftState = m_LeftControl.DigitalStateRead();
		uint8 rightState = m_RightControl.DigitalStateRead();

		if (((rightState & 0b11) == 0b10) && ((leftState & 0b11) == 0b00))
			m_OnRotated(1);
		else if (((leftState & 0b11) == 0b10) && ((rightState & 0b11) == 0b00))
			m_OnRotated(-1);
	}

private:
	Control m_LeftControl;
	Control m_RightControl;

	RotatedEventHandler m_OnRotated;
};

#endif