#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include "Switch.h"

class Button : public Switch
{
public:
	typedef ContextCallback<void, float> HoldEventHandler;

public:
	Button(IHAL *HAL, uint8 Pin, uint16 UpdateRate)
		: Switch(HAL, Pin, UpdateRate)
	{
	}

	void SetOnHoldListener(HoldEventHandler Listener)
	{
		m_OnHold = Listener;
	}

protected:
	void Update(void) override
	{
		Switch::Update();

		if (GetTurnedOn())
			m_OnHold(GetHeldTime());
	}

private:
	HoldEventHandler m_OnHold;
};

#endif