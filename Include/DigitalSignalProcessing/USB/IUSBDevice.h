#pragma once
#ifndef I_USB_DEVICE_H
#define I_USB_DEVICE_H

#include "../DataTypes.h"

class IUSBInterface;

class IUSBDevice
{
public:
	virtual IUSBInterface* GetInterface(uint8_t Index) = 0;

	template<typename T>
	T* GetInterface(uint8_t Index)
	{
		IUSBInterface* interface = GetInterface(Index);
		if (interface == nullptr)
			return nullptr;

		return reinterpret_cast<T*>(interface);
	}
};

#endif