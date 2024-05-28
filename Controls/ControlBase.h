#pragma once
#ifndef CONTROL_BASE_H
#define CONTROL_BASE_H

#include "../Common.h"
#include "../IHAL.h"
#include "../Debug.h"

class ControlBase
{
	template <uint8 MaxControlCount, uint16 ProcessRate>
	friend class ControlFactory;

public:
	ControlBase(IHAL *HAL, uint16 UpdateRate)
		: m_HAL(HAL),
		  m_Enabled(true),
		  m_UpdateStep(1000 / UpdateRate),
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

		uint32 time = m_HAL->GetTimeSinceStartupMs();
		if (time < m_NextUpdateTime)
			return;
		m_NextUpdateTime = time + m_UpdateStep;

		Update();
	}

private:
	IHAL *m_HAL;
	bool m_Enabled;
	uint16 m_UpdateStep;
	uint32 m_NextUpdateTime;
};

#endif