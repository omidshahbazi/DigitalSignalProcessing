#include "DigitalSignalProcessing/Gain.h"
#include "DigitalSignalProcessing/Math.h"

LinearGain::LinearGain(const dBGain& gain)
{
	m_Value = Math::dBToLinear((float)gain);
}

dBGain::dBGain(const LinearGain& gain)
{
	m_Value = Math::LinearTodB((float)gain);
}