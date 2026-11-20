#pragma once
#ifndef I_USB_AMC_INTERFACE_H
#define I_USB_AMC_INTERFACE_H

#include "IUSBInterface.h"
#include "DigitalSignalProcessing/Gain.h"
#include "DigitalSignalProcessing/FixedFunction.h"

class IUSBAMCInterface : public IUSBInterface
{
public:
	enum class ControlTypes : uint8_t
	{
		OutSampleRate = 1,
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
	virtual void Read(NotNull<float> InterleavedBuffer, puint16_t TotalSampleCount) = 0;
	virtual void Read(NotNull<double> InterleavedBuffer, puint16_t TotalSampleCount) = 0;

	virtual void Write(NotNull<const float> InterleavedBuffer, puint16_t TotalSampleCount) = 0;
	virtual void Write(NotNull<const double> const InterleavedBuffer, puint16_t TotalSampleCount) = 0;

	virtual void SetControlChangedCallback(ControlChangedCallback Callback) = 0;

	virtual uint32_t GetCurrentOutSampleRate(void) const = 0;
	virtual uint32_t GetCurrentInSampleRate(void) const = 0;

	virtual uint8_t GetCurrentOutBitDepth(void) const = 0;
	virtual uint8_t GetCurrentInBitDepth(void) const = 0;

	virtual dBGain GetCurrentOutVolume(void) const = 0;
	virtual dBGain GetCurrentInVolume(void) const = 0;

	virtual bool IsOutMuted(void) const = 0;
	virtual bool IsInMuted(void) const = 0;
};

#endif