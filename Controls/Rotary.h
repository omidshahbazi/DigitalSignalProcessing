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
	Rotary(IHAL *HAL, uint8 LeftPin, uint8 RightPin, uint16 UpdateRate)
		: ControlBase(HAL, UpdateRate),
		  m_LeftControl(HAL, LeftPin, IHAL::PinModes::DigitalInput, UpdateRate),
		  m_RightControl(HAL, RightPin, IHAL::PinModes::DigitalInput, UpdateRate),
		  m_PrevLeftValue(false)
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
		bool leftValue = m_LeftControl.DigitalRead();
		bool rightValue = m_RightControl.DigitalRead();

		if (leftValue && !m_PrevLeftValue)
		{
			if (leftValue != rightValue)
			{
				m_OnRotated(1);
			}
			else
			{
				m_OnRotated(-1);
			}
		}

		m_PrevLeftValue = leftValue;
	}

private:
	Control m_LeftControl;
	Control m_RightControl;

	bool m_PrevLeftValue;

	RotatedEventHandler m_OnRotated;
};

#endif