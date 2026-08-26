#include "DigitalSignalProcessing/Controls/Button.h"

Button::Button(IHAL* HAL, uint8 Pin)
	: Switch(HAL, Pin)
{}

void Button::Update(void)
{
	Switch::Update();

	if (GetTurnedOn())
		m_OnHold(GetHeldTime());
}