#pragma once
#ifndef ROTARY_H
#define ROTARY_H

#include "Control.h"

class RotaryButton;

class Rotary : public ControlBase
{
	friend class RotaryButton;

public:
	typedef ContextCallback<void, int8, float> RotatedEventHandler;

public:
	Rotary(IHAL* HAL, uint8 APin, uint8 BPin, uint8 Sensitivity = 50);

	void SetOnRotatedListener(RotatedEventHandler Listener)
	{
		m_OnRotated = Listener;
	}

protected:
	virtual void Update(void) override;

private:
	Control m_AControl;
	Control m_BControl;
	uint8 m_Sensitivity;
	uint32 m_LastChangeTime;

	RotatedEventHandler m_OnRotated;
};

#endif