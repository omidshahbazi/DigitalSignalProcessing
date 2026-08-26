#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include "ControlBase.h"
#include "../ContextCallback.h"

class DualLED;
class TripleLED;
class Rotary;

class Control : public ControlBase
{
	friend class DualLED;
	friend class TripleLED;
	friend class Rotary;

public:
	Control(IHAL* HAL, uint8 Pin, IHAL::PinModes Mode, uint16 UpdateRate);

protected:
	virtual void Update(void) override
	{
	}

	uint8 GetPin(void) const
	{
		return m_Pin;
	}

	float AnalogRead(void) const;

	bool DigitalRead(void) const;

	uint8 DigitalStateRead(void);

	void DigitalWrite(bool Value);

	void PWMWrite(float Value);

private:
	uint8 m_Pin;
	uint8 m_State;
};

#endif