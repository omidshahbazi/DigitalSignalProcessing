#pragma once
#ifndef I_USB_CDC_INTERFACE_H
#define I_USB_CDC_INTERFACE_H

#include "IUSBInterface.h"
#include <DigitalSignalProcessing/FixedFunction.h>

class IUSBCDCInterface : public IUSBInterface
{
public:
	typedef FixedFunction<void(const uint8* Buffer, uint16 Length)> ReceiveCallback;

public:
	virtual bool IsConnected(void) const = 0;

	virtual void SetReceiveCallback(ReceiveCallback Callback) = 0;

	virtual void Transmit(const uint8* Buffer, uint16 Length) = 0;
};

#endif