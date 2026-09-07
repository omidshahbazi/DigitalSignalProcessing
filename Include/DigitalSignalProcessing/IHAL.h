#pragma once
#ifndef I_HAL_H
#define I_HAL_H

class IUSB;

class IHAL
{
public:
	typedef void (*AudioPassthrough)(const float* const* In, float** Out, uint32_t Size);

	enum class PinModes
	{
		AnalogInput = 0,
		DigitalInput,
		DigitalOutput,
		PWM
	};

public:
	virtual void Setup(uint8_t FrameLength, uint32_t SampleRate, bool Boost = false) = 0;
	virtual void StartAudio(AudioPassthrough Callback) = 0;

	virtual void* Allocate(uint32_t Size, bool OnSDRAM = false) = 0;
	virtual void Deallocate(void* Memory) = 0;

	virtual bool IsAnAnalogPin(uint8_t Pin) const = 0;
	virtual bool IsADigitalPin(uint8_t Pin) const = 0;

	virtual bool IsAnInputPin(uint8_t Pin) const = 0;
	virtual bool IsAnOutputPin(uint8_t Pin) const = 0;

	virtual bool IsAPWMPin(uint8_t Pin) const = 0;

	virtual bool IsInInputMode(uint8_t Pin) const = 0;
	virtual bool IsInOutputMode(uint8_t Pin) const = 0;

	virtual void SetPinMode(uint8_t Pin, PinModes Mode) = 0;

	virtual void SetPWMResolution(uint8_t Value) = 0;
	virtual uint8_t GetPWMResolution(void) const = 0;

	virtual float AnalogRead(uint8_t Pin) const = 0;

	virtual bool DigitalRead(uint8_t Pin) const = 0;
	virtual void DigitalWrite(uint8_t Pin, bool Value) = 0;

	virtual void PWMWrite(uint8_t Pin, float Value) = 0;

	virtual uint32_t GetTimeFrequency(void) const = 0;
	virtual uint32_t GetTimeSinceStartupTicks(void) const = 0;
	virtual uint32_t GetTimeSinceStartupMs(void) const = 0;
	virtual float GetTimeSinceStartup(void) const = 0;

	virtual void Print(cstr Value) = 0;

	virtual bool IsDebuggerPresent(void) const = 0;
	virtual void Crash(void) const = 0;
	virtual void Break(void) const = 0;
	virtual void Reset(bool InfiniteTime = true) const = 0;

	virtual void Delay(uint16_t Ms) const = 0;

	virtual IUSB* GetUSB(uint8_t Index = 0) = 0;

	virtual void EraseQSPIData(void) = 0;
};

#endif