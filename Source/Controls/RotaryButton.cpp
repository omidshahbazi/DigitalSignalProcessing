#include "DigitalSignalProcessing/Controls/RotaryButton.h"

RotaryButton::RotaryButton(IHAL* HAL, uint8 APin, uint8 BPin, uint8 ButtonPin)
	: Button(HAL, ButtonPin),
	m_Rotary(HAL, APin, BPin)
{}

void RotaryButton::Update(void)
{
	m_Rotary.Update();

	Button::Update();
}