#pragma once
#ifndef I_HAL_H
#define I_HAL_H

#include "IUSBInterface.h"

class IHAL
{
public:
	typedef void (*AudioPassthrough)(const float *const *In, float **Out, uint32 Size);

	enum class PinModes
	{
		AnalogInput = 0,
		DigitalInput,
		DigitalOutput,
		PWM
	};

public:
	virtual void Setup(uint8 FrameLength, uint32 SampleRate, bool Boost = false, bool USBTransmissionMode = false, bool WaitForDebugger = false) = 0;
	virtual void StartAudio(AudioPassthrough Callback) = 0;

	virtual void *Allocate(uint32 Size, bool OnSDRAM = false) = 0;
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

	virtual void InitializePersistentData(uint16 ID) = 0;
	virtual bool ContainsPersistentData(uint16 ID) = 0;
	virtual void SetPersistentData(uint16 ID, const void *const Data, uint16 Size) = 0;
	virtual void GetPersistentData(uint16 ID, void *Data, uint16 Size) = 0;
	virtual void EreasPersistentData(void) = 0;
	virtual void SavePersistentData(void) = 0;

	virtual uint32 GetTimeSinceStartupMs(void) const = 0;
	virtual float GetTimeSinceStartup(void) const = 0;

	virtual void Print(cstr Value) = 0;

	virtual void Break(void) const = 0;

	virtual void Delay(uint16 Ms) const = 0;

	virtual IUSBInterface* GetUSBInterface(void) = 0;
};

#endif