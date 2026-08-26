#include "DigitalSignalProcessing/Controls/ControlBase.h"
#include "DigitalSignalProcessing/Debug.h"

ControlBase::ControlBase(IHAL* HAL, uint16 UpdateRate)
	: m_HAL(HAL),
	m_Enabled(true),
	m_UpdateStep(1000 / UpdateRate),
	m_NextUpdateTime(0)
{}

void ControlBase::SetPinMode(uint8 Pin, IHAL::PinModes Mode)
{
	m_HAL->SetPinMode(Pin, Mode);
}

float ControlBase::AnalogRead(uint8 Pin) const
{
	return m_HAL->AnalogRead(Pin);
}

bool ControlBase::DigitalRead(uint8 Pin) const
{
	return m_HAL->DigitalRead(Pin);
}

void ControlBase::DigitalWrite(uint8 Pin, bool Value)
{
	m_HAL->DigitalWrite(Pin, Value);
}

void ControlBase::PWMWrite(uint8 Pin, float Value)
{
	ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

	m_HAL->PWMWrite(Pin, Value);
}

void ControlBase::Process(void)
{
	if (!m_Enabled)
		return;

	uint32 time = m_HAL->GetTimeSinceStartupMs();
	if (time < m_NextUpdateTime)
		return;
	m_NextUpdateTime = time + m_UpdateStep;

	Update();
}