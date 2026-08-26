#include "DigitalSignalProcessing/Controls/DualLED.h"

DualLED::DualLED(IHAL* HAL, uint8 RedPin, uint8 GreenPin, uint16 UpdateRate, bool UsePWM)
	: ControlBase(HAL, UpdateRate),
	LEDBase(HAL),
	m_LEDRed(HAL, RedPin, (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput), UpdateRate),
	m_LEDGreen(HAL, GreenPin, (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput), UpdateRate),
	m_UsePWM(UsePWM)
{
	IHAL::PinModes pinMode = (UsePWM ? IHAL::PinModes::PWM : IHAL::PinModes::DigitalOutput);
	SetPinMode(RedPin, pinMode);
	SetPinMode(GreenPin, pinMode);
}

void DualLED::SetColor(const Color& Value)
{
	m_Color = Value;

	m_Color.R = Math::Clamp(m_Color.R, 0, 255);
	m_Color.G = Math::Clamp(m_Color.G, 0, 255);
}

void DualLED::Update(void)
{
	float brightness = LEDBase::GetBrightness();

	if (m_UsePWM)
	{
		m_LEDRed.PWMWrite((m_Color.R / 255.0) * brightness);
		m_LEDGreen.PWMWrite((m_Color.G / 255.0) * brightness);
	}
	else
	{
		m_LEDRed.DigitalWrite(brightness);
		m_LEDGreen.DigitalWrite(brightness);
	}
}