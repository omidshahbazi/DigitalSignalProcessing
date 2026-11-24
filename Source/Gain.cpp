#include "DigitalSignalProcessing/Gain.h"
#include "DigitalSignalProcessing/Math.h"

LinearGain::LinearGain(const dBGain& Gain)
{
	m_Value = Math::dBToLinear((float)Gain);
}

LinearGain LinearGain::Interpolate(LinearGain Min, LinearGain Max, float Time, bool Linear)
{
	if (Linear)
		return (LinearGain)Math::Lerp(Min, Max, Time);

	return (LinearGain)dBGain::Interpolate((dBGain)Min, (dBGain)Max, Time);
}

dBGain::dBGain(const LinearGain& gain)
{
	m_Value = Math::LinearTodB((float)gain);
}

dBGain dBGain::Interpolate(dBGain Min, dBGain Max, float Time)
{
	return (dBGain)Math::Lerp((float)Min, (float)Max, Time);
}