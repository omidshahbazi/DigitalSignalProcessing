#pragma once
#ifndef I_USB_INTERFACE_H
#define I_USB_INTERFACE_H

#include "USBProfile.h"

class ICDCUSB;
class IAMCUSB;
class IMIDIUSB;

class IUSBInterface
{
public:
	virtual void Start(const USBProfile& Profile) = 0;
	virtual void Stop(void) = 0;

	virtual ICDCUSB* GetCDC(uint8 Index) = 0;
	virtual IAMCUSB* GetAMC(void) = 0;
	virtual IMIDIUSB* GetMIDI(void) = 0;
};

#endif