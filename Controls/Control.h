#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include "../Common.h"
#include "../IHAL.h"
#include "../Debug.h"

class ControlFactory;

class Control
{
	friend class ControlFactory;

public:
	Control(IHAL *HAL, uint8 Pin, IHAL::PinModes Mode)
		: m_HAL(HAL),
		  m_Pin(Pin),
		  m_Enabled(true)
	{
		ASSERT(Mode != IHAL::PinModes::Input || m_HAL->IsAnInputPin(Pin), "Pin %i is not an input pin", Pin);
		ASSERT(Mode != IHAL::PinModes::Output || m_HAL->IsAnOutputPin(Pin), "Pin %i is not an output pin", Pin);

		m_HAL->SetPinMode(m_Pin, Mode);
		m_HAL->SetAnalogReadResolution(10);
	}

	void SetEnabled(bool Value)
	{
		m_Enabled = false;
	}
	bool GetEnabled(void) const
	{
		return m_Enabled;
	}

protected:
	virtual void Update(void) = 0;

	uint8 GetPin(void) const
	{
		return m_Pin;
	}

	float AnalogRead(void) const
	{
		return m_HAL->AnalogRead(m_Pin);
	}

	bool DigitalRead(void) const
	{
		return m_HAL->DigitalRead(m_Pin);
	}

	void DigitalWrite(bool Value)
	{
		m_HAL->DigitalWrite(m_Pin, Value);
	}

private:
	void Process(void)
	{
		if (!m_Enabled)
			return;

		Update();
	}

private:
	IHAL *m_HAL;
	uint8 m_Pin;
	bool m_Enabled;
};

#endif