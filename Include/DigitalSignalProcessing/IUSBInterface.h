#pragma once
#ifndef I_USB_INTERFACE_H
#define I_USB_INTERFACE_H

#include "Common.h"
#include "ContextCallback.h"

class IUSBInterface
{
public:
	enum class USBInterfaces
	{
		Internal = 0,
		External
	};

	typedef ContextCallback<void, const uint8 *, uint16> EventHandler;

public:
	virtual void Start(USBInterfaces Interface) = 0;

	virtual void Stop(void) = 0;

	virtual void Transmit(const uint8 *Buffer, uint16 Length) const = 0;

	virtual void SetCallback(EventHandler Callback) = 0;
};

#endif