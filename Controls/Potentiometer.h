#pragma once
#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include "Control.h"
#include "../Log.h"
#include "../Filters/BiquadFilter.h"

class Potentiometer : public Control
{
public:
	typedef ContextCallback<void, float> EventHandler;

public:
	Potentiometer(IHAL *HAL, uint8 Pin, uint16 UpdateRate, bool FilterSwings = false)
		: Control(HAL, Pin, IHAL::PinModes::AnalogInput, UpdateRate),
		  m_Filter(1),
		  m_FilterSwings(FilterSwings),
		  m_Value(-1)
	{
		ASSERT(HAL->IsAnAnaloglPin(Pin), "Pin %i is not an analog pin", Pin);

		if (m_FilterSwings)
			BiquadFilter<float>::SetLowPassFilterCoefficients(&m_Filter, UpdateRate, 0.5);

		SetCalibrationValues(0, 1);
	}

	float GetValue(void) const
	{
		return m_Value;
	}

	void SetOnChangedListener(EventHandler Listener)
	{
		m_OnChanged = Listener;
	}

	void SetCalibrationValues(float Min, float Max)
	{
		m_CalibrationMin = Min;
		m_CalibrationMax = Max;
	}

protected:
	void Update(void) override
	{
		float prevValue = m_Value;

		m_Value = Math::Clamp01(Math::Map(AnalogRead(), m_CalibrationMin, m_CalibrationMax, 0.0F, 1.0F));

		if (m_FilterSwings)
		{
			if (0 < m_Value && m_Value < 1)
				m_Value = Math::Clamp01(m_Filter.Process(m_Value));

			if (Math::Absolute(prevValue - m_Value) < 0.005F)
				return;
		}
		else if (m_Value == prevValue)
			return;

		Log::WriteDebug("Potentiometer", "Potentiometer GPIOPins::Pin%i value: %f, diff %f", (uint8)GetPin(), m_Value, Math::Absolute(prevValue - m_Value));

		m_OnChanged(m_Value);
	}

private:
	BiquadFilter<float> m_Filter;
	bool m_FilterSwings;
	float m_Value;
	float m_CalibrationMin;
	float m_CalibrationMax;
	EventHandler m_OnChanged;
};

#endif