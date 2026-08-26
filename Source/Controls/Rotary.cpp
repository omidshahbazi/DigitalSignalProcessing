#include "DigitalSignalProcessing/Controls/Rotary.h"
#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Math.h"

Rotary::Rotary(IHAL* HAL, uint8 APin, uint8 BPin, uint8 Sensitivity)
	: ControlBase(HAL, 1000),
	m_AControl(HAL, APin, IHAL::PinModes::DigitalInput, 1),
	m_BControl(HAL, BPin, IHAL::PinModes::DigitalInput, 1),
	m_Sensitivity(Sensitivity),
	m_LastChangeTime(0)
{
	ASSERT(HAL->IsADigitalPin(APin), "Pin %i is not an digital pin", APin);
	ASSERT(HAL->IsADigitalPin(BPin), "Pin %i is not an digital pin", BPin);
}

void Rotary::Update(void)
{
	uint8 aState = m_AControl.DigitalStateRead();
	uint8 bState = m_BControl.DigitalStateRead();

	int8 direction = 0;
	if (((bState & 0b11) == 0b10) && ((aState & 0b11) == 0b00))
		direction = -1;
	else if (((aState & 0b11) == 0b10) && ((bState & 0b11) == 0b00))
		direction = 1;
	else
		return;

	uint32 time = GetHAL()->GetTimeSinceStartupMs();
	uint32 deltaTime = Math::Max(1, time - m_LastChangeTime);
	m_LastChangeTime = time;

	float acceleration = Math::Max(1, m_Sensitivity / (float)deltaTime);

	m_OnRotated(direction, acceleration);
}