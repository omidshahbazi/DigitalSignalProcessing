#include "DigitalSignalProcessing/Controls/Rotary.h"
#include "DigitalSignalProcessing/Debug.h"
#include "DigitalSignalProcessing/Math.h"

Rotary::Rotary(IHAL* HAL, uint8_t APin, uint8_t BPin, uint8_t Sensitivity)
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
	uint8_t aState = m_AControl.DigitalStateRead();
	uint8_t bState = m_BControl.DigitalStateRead();

	int8_t direction = 0;
	if (((bState & 0b11) == 0b10) && ((aState & 0b11) == 0b00))
		direction = -1;
	else if (((aState & 0b11) == 0b10) && ((bState & 0b11) == 0b00))
		direction = 1;
	else
		return;

	uint32_t time = GetHAL()->GetTimeSinceStartupMs();
	uint32_t deltaTime = Math::Max(1, time - m_LastChangeTime);
	m_LastChangeTime = time;

	float acceleration = Math::Max(1, m_Sensitivity / (float)deltaTime);

	m_OnRotated(direction, acceleration);
}