#pragma once
#ifndef I_FIRMWARE_H
#define I_FIRMWARE_H

#include "DataTypes.h"

class IFirmware
{
public:
	virtual bool Initialize(void) = 0;
	virtual bool StartUpdate(uint32_t Size) = 0;
	virtual bool WritePage(const uint8_t* Buffer, uint16_t Length, uint32_t Checksum) = 0;
	virtual void EndUpdate(void) = 0;
};

#endif