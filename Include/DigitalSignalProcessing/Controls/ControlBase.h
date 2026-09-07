#pragma once
#ifndef CONTROL_BASE_H
#define CONTROL_BASE_H

#include "../Common.h"
#include "../IHAL.h"

class ControlBase
{
	template <uint8_t MaxControlCount, uint16_t ProcessRate>
	friend class ControlFactory;

public:
	ControlBase(IHAL* HAL, uint16_t UpdateRate);

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

	void SetPinMode(uint8_t Pin, IHAL::PinModes Mode);

	float AnalogRead(uint8_t Pin) const;

	bool DigitalRead(uint8_t Pin) const;

	void DigitalWrite(uint8_t Pin, bool Value);

	//[0, 1]
	void PWMWrite(uint8_t Pin, float Value);

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
	uint16_t m_UpdateStep;
	uint32_t m_NextUpdateTime;
};

#endif