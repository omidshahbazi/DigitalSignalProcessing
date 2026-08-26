#include "DigitalSignalProcessing/Controls/Control.h"

Control::Control(IHAL* HAL, uint8 Pin, IHAL::PinModes Mode, uint16 UpdateRate)
	: ControlBase(HAL, UpdateRate),
	m_Pin(Pin),
	m_State(0)
{
	SetPinMode(m_Pin, Mode);
}

float Control::AnalogRead(void) const
{
	return ControlBase::AnalogRead(m_Pin);
}

bool Control::DigitalRead(void) const
{
	return ControlBase::DigitalRead(m_Pin);
}

uint8 Control::DigitalStateRead(void)
{
	m_State <<= 1;
	m_State |= (DigitalRead() ? 1 : 0);
	return m_State;
}

void Control::DigitalWrite(bool Value)
{
	ControlBase::DigitalWrite(m_Pin, Value);
}

void Control::PWMWrite(float Value)
{
	ControlBase::PWMWrite(m_Pin, Value);
}