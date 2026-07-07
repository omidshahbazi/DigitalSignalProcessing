#pragma once
#ifndef I_USB_AUDIO_INTERFACE_H
#define I_USB_AUDIO_INTERFACE_H

#include "Common.h"

template <typename T>
class IUSBAudioInterface
{
public:
	virtual void Start(void) = 0;

	virtual void Stop(void) = 0;

	virtual void Transmit(const T *BufferL, const T *BufferR, uint8 Count) = 0;
};

#endif