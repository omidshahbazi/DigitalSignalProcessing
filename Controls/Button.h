#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include "Switch.h"
#include <functional>

class Button : public Switch
{
public:
	typedef std::function<void(float)> HoldEventHandler;

public:
	Button(IHAL *HAL, uint8 Pin, uint16 UpdateRate)
		: Switch(HAL, Pin, UpdateRate)
	{
	}

	void SetOnHoldListener(TurnedOffEventHandler &&Listener)
	{
		m_OnHold = Listener;
	}

protected:
	void Update(void) override
	{
		if (GetTurnedOn() && m_OnHold != nullptr)
			m_OnHold(GetHeldTime());
	}

private:
	HoldEventHandler m_OnHold;
};

#endif