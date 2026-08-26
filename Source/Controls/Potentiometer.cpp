#include "DigitalSignalProcessing/Controls/Potentiometer.h"
#include "DigitalSignalProcessing/Log.h"

Potentiometer::Potentiometer(IHAL* HAL, uint8 Pin, uint16 UpdateRate, bool FilterSwings)
	: Control(HAL, Pin, IHAL::PinModes::AnalogInput, UpdateRate),
	m_FilterSwings(FilterSwings),
	m_Value(-1)
{
	ASSERT(HAL->IsAnAnalogPin(Pin), "Pin %i is not an analog pin", Pin);

	if (m_FilterSwings)
		BiquadFilter<float, MIN_SAMPLE_RATE>::SetLowPassCoefficients(&m_Filter, Frequency(UpdateRate), QUALITY_FACTOR_CRITICAL_DAMPING);

	SetCalibrationValues(0, 1);
}

void Potentiometer::Update(void) 
{
	float prevValue = m_Value;

	m_Value = Math::Clamp01(Math::Map(AnalogRead(), m_CalibrationMin, m_CalibrationMax, 0, 1));

	if (m_FilterSwings)
	{
		if (0 < m_Value && m_Value < 1)
		{
			m_Filter.Process(&m_Value, 1);
			m_Value = Math::Clamp01(m_Value);
		}

		if (Math::Absolute(prevValue - m_Value) < 0.005F)
			return;
	}
	else if (m_Value == prevValue)
		return;

	Log::WriteDebug("Potentiometer", "Potentiometer GPIOPins::Pin%i value: %f, diff %f", GetPin(), m_Value, Math::Absolute(prevValue - m_Value));

	m_OnChanged(m_Value);
}