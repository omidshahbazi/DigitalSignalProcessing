#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include "../Common.h"
#include "../IHAL.h"
#include "../Debug.h"
#include "../Time.h"

class ControlFactory;

class Control
{
	friend class ControlFactory;

public:
	Control(IHAL *HAL, uint8 Pin, IHAL::PinModes Mode, uint16 UpdateRate)
		: m_HAL(HAL),
		  m_Pin(Pin),
		  m_Enabled(true),
		  m_UpdateRate(UpdateRate)
	{
		ASSERT(Mode != IHAL::PinModes::Input || m_HAL->IsAnInputPin(Pin), "Pin %i is not an input pin", Pin);
		ASSERT(Mode != IHAL::PinModes::Output || m_HAL->IsAnOutputPin(Pin), "Pin %i is not an output pin", Pin);

		m_HAL->SetPinMode(m_Pin, Mode);
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

		if (Time::GetNow() < m_NextUpdateTime)
			return;
		m_NextUpdateTime = Time::GetNow() + (1.0F / m_UpdateRate);

		Update();
	}

private:
	IHAL *m_HAL;
	uint8 m_Pin;
	bool m_Enabled;
	uint16 m_UpdateRate;
	float m_NextUpdateTime;
};

#endif