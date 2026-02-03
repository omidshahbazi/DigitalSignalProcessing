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
	Rotary(IHAL *HAL, uint8 APin, uint8 BPin)
		: ControlBase(HAL, 1000),
		  m_AControl(HAL, APin, IHAL::PinModes::DigitalInput, 1),
		  m_BControl(HAL, BPin, IHAL::PinModes::DigitalInput, 1)
	{
		ASSERT(HAL->IsADigitalPin(APin), "Pin %i is not an digital pin", APin);
		ASSERT(HAL->IsADigitalPin(BPin), "Pin %i is not an digital pin", BPin);
	}

	void SetOnRotatedListener(RotatedEventHandler Listener)
	{
		m_OnRotated = Listener;
	}

protected:
	virtual void Update(void) override
	{
		uint8 aState = m_AControl.DigitalStateRead();
		uint8 bState = m_BControl.DigitalStateRead();

		if (((bState & 0b11) == 0b10) && ((aState & 0b11) == 0b00))
			m_OnRotated(-1);
		else if (((aState & 0b11) == 0b10) && ((bState & 0b11) == 0b00))
			m_OnRotated(1);
	}

private:
	Control m_AControl;
	Control m_BControl;

	RotatedEventHandler m_OnRotated;
};

#endif