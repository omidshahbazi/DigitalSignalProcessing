#pragma once
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "DataTypes.h"
#include "Debug.h"

template<typename T, uint8_t Alignment = 16, bool IsResettable = true>
class Allocator
{
public:
	Allocator(void* Buffer = nullptr, uint32_t Size = 0)
		: m_Buffer(reinterpret_cast<uint8_t*>(Buffer)),
		m_Capacity(Size),
		m_LastFreeIndex(0)
	{
		ASSERT(Size == 0 || Buffer != nullptr, "Buffer cannot be null");
		ASSERT(Buffer == nullptr || Size > 0, "Size cannot be zero");
	}

	T* Allocate(uint32_t Count)
	{
		return Allocate<T>(Count);
	}

	template<typename ElementType, uint32_t ElementSize = sizeof(ElementType)>
	ElementType* Allocate(uint32_t Count = 1)
	{
		const uint32_t totalSize = Count * ElementSize;

		ASSERT(m_Buffer != nullptr, "Allocator is not initialized");

		uintptr_t ptr = reinterpret_cast<uintptr_t>(m_Buffer) + m_LastFreeIndex;
		uintptr_t alignedPtr = (ptr + (Alignment - 1)) & ~(Alignment - 1);
		uint8_t padding = alignedPtr - ptr;

		ASSERT(m_LastFreeIndex + totalSize + padding <= m_Capacity, "Running out of memory Capacity: %ub, Allocated: %ub Requested: %ub", m_Capacity, m_LastFreeIndex, totalSize);

		m_LastFreeIndex += totalSize + padding;

		return reinterpret_cast<ElementType*>(alignedPtr);
	}

	void Reset(void)
	{
		if constexpr (IsResettable)
			m_LastFreeIndex = 0;
		else
			BREAK("Allocator is not resettable");
	}

	bool Contains(void* Address) const
	{
		return (m_Buffer <= Address && Address < m_Buffer + m_Capacity);
	}

	uint32_t GetCapacity(void) const
	{
		return m_Capacity;
	}

	uint32_t GetTotalAllocated(void) const
	{
		return m_LastFreeIndex;
	}

private:
	uint8_t* m_Buffer;
	uint32_t m_Capacity;
	uint32_t m_LastFreeIndex;
};

#endif