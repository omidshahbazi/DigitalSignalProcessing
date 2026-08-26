#pragma once
#ifndef I_USB_AUDIO_INTERFACE_H
#define I_USB_AUDIO_INTERFACE_H

#include "Common.h"

class IUSBAudioInterface
{
public:
	enum class USBInterfaces
	{
		Internal = 0,
		External
	};

public:
	virtual void Start(USBInterfaces Interface) = 0;

	virtual void Stop(void) = 0;

	virtual void Push(const uint8* BufferL, const uint8* BufferR, uint8 Count) = 0;

	template<typename T>
	void PushFrame(const T* FrameBufferL, const T* FrameBufferR, uint8 Length)
	{
		Push(reinterpret_cast<const uint8*>(FrameBufferL), reinterpret_cast<const uint8*>(FrameBufferR), Length);
	}
};

#endif