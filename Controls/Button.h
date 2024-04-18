#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include "Control.h"
#include <functional>

class Button : public Control
{
public:
	typedef std::function<void(bool)> OnStateChangedEventHandler;
	typedef std::function<void(void)> TurnedOnEventHandler;
	typedef std::function<void(float)> HoldEventHandler;
	typedef std::function<void(float)> TurnedOffEventHandler;

public:
	Button(IHAL *HAL, uint8 Pin, uint16 UpdateRate)
		: Control(HAL, Pin, IHAL::PinModes::Input, UpdateRate),
		  m_TurnedOn(false),
		  m_TurnedOnTime(0)
	{
		ASSERT(HAL->IsADigitalPin(Pin), "Pin %i is not an digital pin", Pin);

		Update();
	}

	void SetOnStateChangedListener(OnStateChangedEventHandler &&Listener)
	{
		m_OnStateChanged = Listener;
	}

	void SetOnTurnedOnListener(TurnedOnEventHandler &&Listener)
	{
		m_OnTurnedOn = Listener;
	}

	void SetOnTurnedOffListener(HoldEventHandler &&Listener)
	{
		m_OnTurnedOff = Listener;
	}

	void SetOnHoldListener(TurnedOffEventHandler &&Listener)
	{
		m_OnHold = Listener;
	}

	bool GetTurnedOn(void) const
	{
		return m_TurnedOn;
	}

	float GetHeldTime(void) const
	{
		if (!m_TurnedOn)
			return 0;

		return GetHAL()->GetTimeSinceStartup() - m_TurnedOnTime;
	}

protected:
	void Update(void) override
	{
		bool newValue = DigitalRead();
		if (m_TurnedOn == newValue)
		{
			if (m_TurnedOn && m_OnHold != nullptr)
				m_OnHold(GetHeldTime());

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
			m_OnTurnedOff(GetHeldTime());

		if (m_OnStateChanged != nullptr)
			m_OnStateChanged(m_TurnedOn);
	}

private:
	bool m_TurnedOn;
	float m_TurnedOnTime;
	OnStateChangedEventHandler m_OnStateChanged;
	TurnedOnEventHandler m_OnTurnedOn;
	HoldEventHandler m_OnTurnedOff;
	TurnedOffEventHandler m_OnHold;
};

#endif