#ifndef __GNUC__
#include <Windows.h>
#endif

#include "DigitalSignalProcessing/Debug.h"

Debug::CrashHandler Debug::s_Handler = nullptr;

void Debug::SetCrashHandler(CrashHandler Handler)
{
	s_Handler = Handler;
}

bool Debug::IsDebuggerPresent(void)
{
#ifdef __GNUC__
	//const bool IsDebuggerConnected = CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk;
	return true;
#else
	return ::IsDebuggerPresent();
#endif

	return false;
}

void Debug::Crash(void)
{
	s_Handler();
}

void Debug::Break(void)
{
#ifdef __GNUC__
	asm("bkpt 255");

	while (1);
#else
	__debugbreak();
#endif
}
