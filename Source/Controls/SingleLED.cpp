#include "DigitalSignalProcessing/Controls/SingleLED.h"

SingleLED::SingleLED(IHAL* HAL, uint8_t Pin, uint16_t UpdateRate, bool UsePWM)
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