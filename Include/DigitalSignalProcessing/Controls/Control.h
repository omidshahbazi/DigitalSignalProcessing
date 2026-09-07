#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include "ControlBase.h"
#include "../FixedFunction.h"

class DualLED;
class TripleLED;
class Rotary;

class Control : public ControlBase
{
	friend class DualLED;
	friend class TripleLED;
	friend class Rotary;

public:
	Control(IHAL* HAL, uint8_t Pin, IHAL::PinModes Mode, uint16_t UpdateRate);

protected:
	virtual void Update(void) override
	{
	}

	uint8_t GetPin(void) const
	{
		return m_Pin;
	}

	float AnalogRead(void) const;

	bool DigitalRead(void) const;

	uint8_t DigitalStateRead(void);

	void DigitalWrite(bool Value);

	void PWMWrite(float Value);

private:
	uint8_t m_Pin;
	uint8_t m_State;
};

#endif