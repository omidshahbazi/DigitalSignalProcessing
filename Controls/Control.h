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
		  m_Pin(Pin),
		  m_State(0)
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

	uint8 DigitalStateRead(void)
	{
		m_State <<= 1;
		m_State |= (DigitalRead() ? 1 : 0);
		return m_State;
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
	uint8 m_State;
};

#endif