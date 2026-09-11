#pragma once
#ifndef I_USB_CDC_INTERFACE_H
#define I_USB_CDC_INTERFACE_H

#include "IUSBInterface.h"
#include <DigitalSignalProcessing/FixedFunction.h>

class IUSBCDCInterface : public IUSBInterface
{
public:
	enum class TransmitStates
	{
		Idle = 0,
		Busy
	};

	typedef FixedFunction<void(const uint8_t* Buffer, uint8_t Length)> ReceiveCallback;
	typedef FixedFunction<void(void)> StateChangedCallback;

public:
	virtual ~IUSBCDCInterface(void) = default;

	virtual bool IsConnected(void) const = 0;
	virtual void SetConnectionStateChangedCallback(StateChangedCallback Callback) = 0;

	virtual TransmitStates GetTransmitState(void) const = 0;
	virtual void SetTransmitStateChangedCallback(StateChangedCallback Callback) = 0;

	virtual void SetReceiveCallback(ReceiveCallback Callback) = 0;

	virtual void Transmit(const uint8_t* Buffer, uint16_t Length) = 0;
};

#endif