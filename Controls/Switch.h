#pragma once
#ifndef SWITCH_H
#define SWITCH_H

#include "Control.h"
#include <functional>

class Switch : public Control
{
public:
	typedef std::function<void(bool)> OnStateChangedEventHandler;
	typedef std::function<void(void)> TurnedOnEventHandler;
	typedef std::function<void(float)> TurnedOffEventHandler;

public:
	Switch(IHAL *HAL, uint8 Pin, uint16 UpdateRate)
		: Control(HAL, Pin, IHAL::PinModes::DigitalInput, UpdateRate),
		  m_TurnedOn(false),
		  m_TurnedOnTime(0),
		  m_HeldTime(0)
	{
		ASSERT(HAL->IsADigitalPin(Pin), "Pin %i is not an digital pin", Pin);

		m_TurnedOn = DigitalRead();
	}

	void SetOnStateChangedListener(OnStateChangedEventHandler &&Listener)
	{
		m_OnStateChanged = Listener;
	}

	void SetOnTurnedOnListener(TurnedOnEventHandler &&Listener)
	{
		m_OnTurnedOn = Listener;
	}

	void SetOnTurnedOffListener(TurnedOffEventHandler &&Listener)
	{
		m_OnTurnedOff = Listener;
	}

	bool GetTurnedOn(void) const
	{
		return m_TurnedOn;
	}

	float GetHeldTime(void) const
	{
		return m_HeldTime;
	}

protected:
	virtual void Update(void) override
	{
		bool newValue = DigitalRead();

		if (m_TurnedOn == newValue)
		{
			m_HeldTime = GetHAL()->GetTimeSinceStartup() - m_TurnedOnTime;

			return;
		}

		m_TurnedOn = newValue;

		if (m_TurnedOn)
		{
			m_TurnedOnTime = GetHAL()->GetTimeSinceStartup();

			if (m_OnTurnedOn != nullptr)
				m_OnTurnedOn();
		}
		else if (m_OnTurnedOff != nullptr)
			m_OnTurnedOff(m_HeldTime);

		if (m_OnStateChanged != nullptr)
			m_OnStateChanged(m_TurnedOn);

		if (!m_TurnedOn)
		{
			m_HeldTime = 0;
		}
	}

private:
	bool m_TurnedOn;
	float m_TurnedOnTime;
	float m_HeldTime;
	OnStateChangedEventHandler m_OnStateChanged;
	TurnedOnEventHandler m_OnTurnedOn;
	TurnedOffEventHandler m_OnTurnedOff;
};

#endif