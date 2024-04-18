#pragma once
#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include "Control.h"
#include "../Log.h"
#include "../Filters/BiquadFilter.h"
#include <functional>

class Potentiometer : public Control
{
public:
	typedef std::function<void(float)> EventHandler;

public:
	Potentiometer(IHAL *HAL, uint8 Pin, uint16 UpdateRate, bool FilterSwings = false)
		: Control(HAL, Pin, IHAL::PinModes::Input, UpdateRate),
		  m_Filter(1),
		  m_FilterSwings(FilterSwings),
		  m_Value(-1)
	{
		ASSERT(HAL->IsAnAnaloglPin(Pin), "Pin %i is not an analog pin", Pin);

		if (m_FilterSwings)
			BiquadFilter<float>::SetLowPassFilterCoefficients(&m_Filter, UpdateRate, 0.5);
	}

	float GetValue(void) const
	{
		return m_Value;
	}

	void SetOnChangedListener(EventHandler &&Listener)
	{
		m_OnChanged = Listener;
	}

protected:
	void Update(void) override
	{
		float prevValue = m_Value;

		m_Value = AnalogRead();

		if (m_FilterSwings)
		{
			if (0 < m_Value && m_Value < 1)
				m_Value = Math::Clamp01(m_Filter.Process(m_Value));

			if (abs(prevValue - m_Value) < 0.005F)
				return;
		}
		else if (m_Value == prevValue)
			return;

		Log::WriteDebug("Potentiometer", "Potentiometer GPIOPins::Pin%i value: %f, diff %f", (uint8)GetPin(), m_Value, abs(prevValue - m_Value));

		if (m_OnChanged != nullptr)
			m_OnChanged(m_Value);
	}

private:
	BiquadFilter<float> m_Filter;
	bool m_FilterSwings;
	float m_Value;
	EventHandler m_OnChanged;
};

#endif