#pragma once
#ifndef I_USB_H
#define I_USB_H

#include "USBProfile.h"

class IUSBDevice;
class IUSBHost;

class IUSB
{
public:
	virtual void Start(const USBProfile& Profile) = 0;
	virtual void Stop(void) = 0;

	virtual IUSBDevice* GetDevice(void) = 0;
	virtual IUSBHost* GetHost(void) = 0;
};

#endif