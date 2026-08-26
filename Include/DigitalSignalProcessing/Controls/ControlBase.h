#pragma once
#ifndef CONTROL_BASE_H
#define CONTROL_BASE_H

#include "../Common.h"
#include "../IHAL.h"

class ControlBase
{
	template <uint8 MaxControlCount, uint16 ProcessRate>
	friend class ControlFactory;

public:
	ControlBase(IHAL* HAL, uint16 UpdateRate);

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

	void SetPinMode(uint8 Pin, IHAL::PinModes Mode);

	float AnalogRead(uint8 Pin) const;

	bool DigitalRead(uint8 Pin) const;

	void DigitalWrite(uint8 Pin, bool Value);

	//[0, 1]
	void PWMWrite(uint8 Pin, float Value);

protected:
	IHAL* GetHAL(void) const
	{
		return m_HAL;
	}

private:
	void Process(void);

private:
	IHAL* m_HAL;
	bool m_Enabled;
	uint16 m_UpdateStep;
	uint32 m_NextUpdateTime;
};

#endif