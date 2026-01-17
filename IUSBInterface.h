#pragma once
#ifndef I_USB_INTERFACE_H
#define I_USB_INTERFACE_H

#include "Common.h"
#include "ContextCallback.h"

class IUSBInterface
{
public:
	typedef ContextCallback<void, const uint8*, uint32> EventHandler;

public:
	virtual void Transmit(const uint8* Buffer, uint32 Length) = 0;

	virtual void SetCallback(EventHandler Callback) = 0;
};

#endif