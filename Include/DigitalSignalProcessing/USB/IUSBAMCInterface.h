#pragma once
#ifndef I_USB_AMC_INTERFACE_H
#define I_USB_AMC_INTERFACE_H

#include "IUSBInterface.h"
#include "../Gain.h"
#include <DigitalSignalProcessing/FixedFunction.h>

class IUSBAMCInterface : public IUSBInterface
{
public:
	enum class ControlTypes : uint8
	{
		OutSampleRate = 0,
		InSampleRate,
		OutBitDepth,
		InBitDepth,
		OutMute,
		InMute,
		OutVolume,
		InVolume
	};

public:
	typedef FixedFunction<void(ControlTypes Type)> ControlChangedCallback;

public:
	virtual void Read(float* InterleavedBuffer, uint16 TotalSampleCount) = 0;
	virtual void Read(double* InterleavedBuffer, uint16 TotalSampleCount) = 0;

	virtual void Write(const float* const InterleavedBuffer, uint16 TotalSampleCount) = 0;
	virtual void Write(const double* const InterleavedBuffer, uint16 TotalSampleCount) = 0;

	virtual void SetControlChangedCallback(ControlChangedCallback Callback) = 0;

	virtual uint32 GetCurrentOutSampleRate(void) const = 0;
	virtual uint32 GetCurrentInSampleRate(void) const = 0;

	virtual uint8 GetCurrentOutBitDepth(void) const = 0;
	virtual uint8 GetCurrentInBitDepth(void) const = 0;

	virtual dBGain GetCurrentOutVolume(void) const = 0;
	virtual dBGain GetCurrentInVolume(void) const = 0;

	virtual bool GetIsOutMuted(void) const = 0;
	virtual bool GetIsInMuted(void) const = 0;
};

#endif