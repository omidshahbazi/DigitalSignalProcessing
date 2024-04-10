#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include "Common.h"
#include "Debug.h"
#include <memory.h>

class Memory
{
public:
	static void Initialize(IHAL *HAL)
	{
		GetHAL() = HAL;
	}

	template <typename T>
	static T *Allocate(uint16 Count = 1, bool FromExternalRAM = false)
	{
		uint16 length = sizeof(T) * Count;

		T *mem = reinterpret_cast<T *>(GetHAL()->Allocate(length));

		ASSERT(mem != nullptr, "Couldn't allocate memory: %i of %iB", Count, sizeof(T));

		Set(mem, 0, Count);

		return mem;
	}

	template <typename T>
	static void Deallocate(T *Memory)
	{
		GetHAL()->Deallocate(Memory);
	}

	template <typename T>
	static void Set(T *Memory, int32 Value, uint16 Count = 1)
	{
		memset(Memory, Value, sizeof(T) * Count);
	}

	template <typename T>
	static void Copy(const T *Source, T *Destination, uint16 Count = 1)
	{
		memcpy(Destination, Source, sizeof(T) * Count);
	}

private:
	static IHAL *&GetHAL(void)
	{
		static IHAL *hal;

		return hal;
	}
};
#endif