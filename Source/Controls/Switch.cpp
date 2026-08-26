#include "DigitalSignalProcessing/Controls/Switch.h"
#include "DigitalSignalProcessing/Debug.h"

Switch::Switch(IHAL* HAL, uint8 Pin)
	: Control(HAL, Pin, IHAL::PinModes::DigitalInput, 1000),
	m_TurnedOn(false),
	m_TurnedOnTime(0),
	m_HeldTime(0)
{
	ASSERT(HAL->IsADigitalPin(Pin), "Pin %i is not an digital pin", Pin);

	m_TurnedOn = DigitalRead();
}

void Switch::Update(void)
{
	bool newValue = (DigitalStateRead() == (uint8)-1);

	if (m_TurnedOn == newValue)
	{
		if (m_TurnedOn)
			m_HeldTime = GetHAL()->GetTimeSinceStartup() - m_TurnedOnTime;

		return;
	}

	m_TurnedOn = newValue;

	if (m_TurnedOn)
	{
		m_TurnedOnTime = GetHAL()->GetTimeSinceStartup();
		m_HeldTime = 0;

		m_OnTurnedOn();
	}
	else
		m_OnTurnedOff(m_HeldTime);

	m_OnStateChanged(m_TurnedOn);

	if (!m_TurnedOn)
	{
		m_HeldTime = 0;
	}
}