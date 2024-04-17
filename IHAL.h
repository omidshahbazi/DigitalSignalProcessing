#pragma once
#ifndef I_HAL_H
#define I_HAL_H

class IHAL
{
public:
	enum class PinModes
	{
		Input = 0,
		Output,
		PWM
	};

public:
	virtual void *Allocate(uint16 Size) = 0;
	virtual void Deallocate(void *Memory) = 0;

	virtual bool IsAnAnaloglPin(uint8 Pin) const = 0;
	virtual bool IsADigitalPin(uint8 Pin) const = 0;

	virtual bool IsAnInputPin(uint8 Pin) const = 0;
	virtual bool IsAnOutputPin(uint8 Pin) const = 0;

	virtual bool IsAPWMPin(uint8 Pin) const = 0;

	virtual void SetPinMode(uint8 Pin, PinModes Mode) = 0;

	virtual void SetPWMResolution(uint8 Value) = 0;
	virtual uint8 GetPWMResolution(void) const = 0;

	virtual float AnalogRead(uint8 Pin) const = 0;

	virtual bool DigitalRead(uint8 Pin) const = 0;
	virtual void DigitalWrite(uint8 Pin, bool Value) = 0;

	virtual void PWMWrite(uint8 Pin, float Value) = 0;

	virtual void Print(const char *Value) const = 0;
	virtual void Break(void) const = 0;

	virtual void Delay(uint16 Ms) const = 0;
};

#endif