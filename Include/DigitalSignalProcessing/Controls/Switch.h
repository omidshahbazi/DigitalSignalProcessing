#pragma once
#ifndef SWITCH_H
#define SWITCH_H

#include "Control.h"

class Switch : public Control
{
public:
	typedef ContextCallback<void, bool> StateChangedEventHandler;
	typedef ContextCallback<void> TurnedOnEventHandler;
	typedef ContextCallback<void, float> TurnedOffEventHandler;

public:
	Switch(IHAL* HAL, uint8 Pin);

	void SetOnStateChangedListener(StateChangedEventHandler Listener)
	{
		m_OnStateChanged = Listener;
	}

	void SetOnTurnedOnListener(TurnedOnEventHandler Listener)
	{
		m_OnTurnedOn = Listener;
	}

	void SetOnTurnedOffListener(TurnedOffEventHandler Listener)
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
	virtual void Update(void) override;

private:
	bool m_TurnedOn;
	float m_TurnedOnTime;
	float m_HeldTime;
	StateChangedEventHandler m_OnStateChanged;
	TurnedOnEventHandler m_OnTurnedOn;
	TurnedOffEventHandler m_OnTurnedOff;
};

#endif