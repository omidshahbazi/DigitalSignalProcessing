#pragma once
#ifndef CONTROL_BASE_H
#define CONTROL_BASE_H

#include "../Common.h"
#include "../IHAL.h"
#include "../Debug.h"

class ControlFactory;

class ControlBase
{
	friend class ControlFactory;

public:
	ControlBase(IHAL *HAL, uint16 UpdateRate)
		: m_HAL(HAL),
		  m_Enabled(true),
		  m_UpdateRate(UpdateRate),
		  m_NextUpdateTime(0)
	{
	}

	virtual void SetEnabled(bool Value)
	{
		m_Enabled = false;
	}
	bool GetEnabled(void) const
	{
		return m_Enabled;
	}

protected:
	virtual void Update(void) = 0;

	void SetPinMode(uint8 Pin, IHAL::PinModes Mode)
	{
		m_HAL->SetPinMode(Pin, Mode);
	}

	float AnalogRead(uint8 Pin) const
	{
		return m_HAL->AnalogRead(Pin);
	}

	bool DigitalRead(uint8 Pin) const
	{
		return m_HAL->DigitalRead(Pin);
	}

	void DigitalWrite(uint8 Pin, bool Value)
	{
		m_HAL->DigitalWrite(Pin, Value);
	}

	//[0, 1]
	void PWMWrite(uint8 Pin, float Value)
	{
		ASSERT(0 <= Value && Value <= 1, "Invalid Value");

		m_HAL->PWMWrite(Pin, Value);
	}

protected:
	IHAL *GetHAL(void) const
	{
		return m_HAL;
	}

private:
	void Process(void)
	{
		if (!m_Enabled)
			return;

		float time = m_HAL->GetTimeSinceStartup();
		if (time < m_NextUpdateTime)
			return;
		m_NextUpdateTime = time + (1.0F / m_UpdateRate);

		Update();
	}

private:
	IHAL *m_HAL;
	bool m_Enabled;
	uint16 m_UpdateRate;
	float m_NextUpdateTime;
};

#endif