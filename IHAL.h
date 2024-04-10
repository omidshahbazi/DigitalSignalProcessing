#pragma once
#ifndef I_HAL_H
#define I_HAL_H

class IHAL
{
public:
	enum class PinModes
	{
		Input = 0,
		Output
	};

public:
	virtual void *Allocate(uint16 Size) = 0;
	virtual void Deallocate(void *Memory) = 0;

	virtual bool IsAnAnaloglPin(int8 Pin) const = 0;
	virtual bool IsADigitalPin(int8 Pin) const = 0;

	virtual bool IsAnInputPin(int8 Pin) const = 0;
	virtual bool IsAnOutputPin(int8 Pin) const = 0;

	virtual void SetPinMode(int8 Pin, PinModes Mode) = 0;

	virtual void SetAnalogReadResolution(int8 Bits) = 0;

	virtual float AnalogRead(int8 Pin) const = 0;
	virtual void AnalogWrite(int8 Pin, float Value) = 0;

	virtual bool DigitalRead(int8 Pin) const = 0;
	virtual void DigitalWrite(int8 Pin, bool Value) = 0;

	virtual void Print(const char *Value) const = 0;
	virtual void Break(void) const = 0;
};

#endif