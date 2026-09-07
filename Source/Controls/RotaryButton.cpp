#include "DigitalSignalProcessing/Controls/RotaryButton.h"

RotaryButton::RotaryButton(IHAL* HAL, uint8_t APin, uint8_t BPin, uint8_t ButtonPin)
	: Button(HAL, ButtonPin),
	m_Rotary(HAL, APin, BPin)
{}

void RotaryButton::Update(void)
{
	m_Rotary.Update();

	Button::Update();
}