#pragma once
#ifndef I_USB_H
#define I_USB_H

#include "USBProfile.h"

class IUSBInterface;

class IUSB
{
public:
	virtual void Start(const USBProfile& Profile) = 0;
	virtual void Stop(void) = 0;

	virtual IUSBInterface* GetInterface(uint8 Index) = 0;

	template<typename T>
	T* GetInterface(uint8 Index)
	{
		IUSBInterface* interface = GetInterface(Index);
		if (interface == nullptr)
			return nullptr;

		return reinterpret_cast<T*>(interface);
	}
};

#endif