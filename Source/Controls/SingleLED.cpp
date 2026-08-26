#include "DigitalSignalProcessing/Controls/SingleLED.h"

SingleLED::SingleLED(IHAL* HAL, uint8 Pin, uint16 UpdateRate, bool UsePWM)
	: Control(HAL, Pin, (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput), UpdateRate),
	LEDBase(HAL),
	m_UsePWM(UsePWM)
{}

void SingleLED::Update(void)
{
	if (m_UsePWM)
		PWMWrite(LEDBase::GetBrightness());
	else
		DigitalWrite(LEDBase::GetBrightness());
}