#include "DigitalSignalProcessing/Log.h"

Log::Callback Log::s_Callback = nullptr;
Log::Types Log::s_Mask = Log::Types::General;

void Log::Initialize(Callback Callback)
{
	s_Callback = Callback;
}

void Log::SetMask(Types Mask)
{
	s_Mask = Mask;
}