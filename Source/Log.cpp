#include "DigitalSignalProcessing/Log.h"

Log::Callback Log::s_Callback = nullptr;
Log::Types Log::s_Mask = Log::Types::General;

bool Log::Initialize(Callback Callback)
{
	s_Callback = Callback;

	return true;
}

bool Log::Deinitialize(void)
{
	s_Callback = nullptr;

	return true;
}

void Log::SetMask(Types Mask)
{
	s_Mask = Mask;
}