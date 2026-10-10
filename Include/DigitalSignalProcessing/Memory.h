#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include "DataTypes.h"
#include "Debug.h"
#include "IHAL.h"
#include <memory.h>

class Memory
{
public:
	static void Initialize(IHAL* HAL);

	template <typename T>
	static T* Allocate(uint32_t Count = 1, bool OnSDRAM = false)
	{
		uint32_t length = sizeof(T) * Count;

		T* mem = reinterpret_cast<T*>(s_HAL->Allocate(length, OnSDRAM));

		ASSERT(mem != nullptr, "Couldn't allocate memory: %i of %iB", Count, sizeof(T));

		Set(mem, 0, Count);

		return mem;
	}

	template <typename T>
	static void Deallocate(T* Memory)
	{
		s_HAL->Deallocate(Memory);
	}

	template <typename T>
	static void Set(T* Memory, int32_t Value, uint32_t Count = 1)
	{
		memset(reinterpret_cast<void*>(Memory), Value, sizeof(T) * Count);
	}

	template <typename T>
	static void Copy(const T* const Source, T* Destination, uint32_t Count = 1)
	{
		memcpy(Destination, Source, sizeof(T) * Count);
	}

	template<typename T>
	static void Swap(T& First, T& Second)
	{
		T temp = Second;
		Second = First;
		First = temp;
	}

	template<typename T, uint8_t Alignment = alignof(T)>
	static T* Align(const T* Pointer)
	{
		return Align<T>(Pointer, Alignment);
	}

	template<typename T>
	static T* Align(const T* Pointer, uint8_t Alignment)
	{
		return reinterpret_cast<T*>(Align(reinterpret_cast<const void*>(Pointer), Alignment));
	}

	template<uint8_t Alignment>
	static void* Align(const void* Pointer)
	{
		return Align(Pointer, Alignment);
	}

	static void* Align(const void* Pointer, uint8_t Alignment)
	{
		uintptr_t alignedPtr = (reinterpret_cast<uintptr_t>(Pointer) + (Alignment - 1)) & ~static_cast<uintptr_t>(Alignment - 1);

		return reinterpret_cast<void*>(alignedPtr);
	}

	template<uint8_t Alignment, typename T>
	static constexpr bool IsAligned(const T* Pointer)
	{
		return IsAligned(Pointer, Alignment);
	}

	template<typename T>
	static constexpr bool IsAligned(const T* Pointer, uint8_t Alignment)
	{
		return ((reinterpret_cast<uintptr_t>(Pointer) & (Alignment - 1)) == 0);
	}

private:
	static IHAL* s_HAL;
};

#endif