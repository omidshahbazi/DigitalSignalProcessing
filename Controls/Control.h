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
	Control(IHAL *HAL, uint8 Pin, IHAL::PinModes Mode, uint16 UpdateRate)
		: ControlBase(HAL, UpdateRate),
		  m_Pin(Pin)
	{
		SetPinMode(m_Pin, Mode);
	}

protected:
	virtual void Update(void) override
	{
	}

	uint8 GetPin(void) const
	{
		return m_Pin;
	}

	float AnalogRead(void) const
	{
		return ControlBase::AnalogRead(m_Pin);
	}

	bool DigitalRead(void) const
	{
		return ControlBase::DigitalRead(m_Pin);
	}

	void DigitalWrite(bool Value)
	{
		ControlBase::DigitalWrite(m_Pin, Value);
	}

	void PWMWrite(float Value)
	{
		ControlBase::PWMWrite(m_Pin, Value);
	}

private:
	uint8 m_Pin;
};

#endif