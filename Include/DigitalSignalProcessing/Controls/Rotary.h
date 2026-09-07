#pragma once
#ifndef ROTARY_H
#define ROTARY_H

#include "Control.h"

class RotaryButton;

class Rotary : public ControlBase
{
	friend class RotaryButton;

public:
	typedef FixedFunction<void(int8_t, float)> RotatedEventHandler;

public:
	Rotary(IHAL* HAL, uint8_t APin, uint8_t BPin, uint8_t Sensitivity = 50);

	void SetOnRotatedListener(RotatedEventHandler Listener)
	{
		m_OnRotated = Listener;
	}

protected:
	virtual void Update(void) override;

private:
	Control m_AControl;
	Control m_BControl;
	uint8_t m_Sensitivity;
	uint32_t m_LastChangeTime;

	RotatedEventHandler m_OnRotated;
};

#endif