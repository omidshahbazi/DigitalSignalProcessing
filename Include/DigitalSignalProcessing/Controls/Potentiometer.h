#pragma once
#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include "Control.h"
#include "../Filters/BiquadFilter.h"

class Potentiometer : public Control
{
public:
	typedef ContextCallback<void, float> EventHandler;

public:
	Potentiometer(IHAL* HAL, uint8 Pin, uint16 UpdateRate, bool FilterSwings = false);

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
	void Update(void) override;

private:
	BiquadFilter<float, MIN_SAMPLE_RATE> m_Filter;
	bool m_FilterSwings;
	float m_Value;
	float m_CalibrationMin;
	float m_CalibrationMax;
	EventHandler m_OnChanged;
};

#endif