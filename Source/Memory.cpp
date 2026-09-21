#include "DigitalSignalProcessing/Memory.h"

IHAL* Memory::s_HAL = nullptr;

void Memory::Initialize(IHAL* HAL)
{
	s_HAL = HAL;
}