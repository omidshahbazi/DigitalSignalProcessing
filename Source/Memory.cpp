#include "DigitalSignalProcessing/Memory.h"

IHAL* Memory::s_HAL = nullptr;

bool Memory::Initialize(IHAL* HAL)
{
	ASSERT(HAL != nullptr, "HAL cannot be null");
	ASSERT(s_HAL == nullptr, "Memory is already initialized");

	s_HAL = HAL;

	return true;
}

bool Memory::Deinitialize(void)
{
	ASSERT(s_HAL != nullptr, "memory is not initialized");

	s_HAL = nullptr;

	return true;
}