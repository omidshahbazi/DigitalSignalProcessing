#pragma once
#ifndef CONTROL_FACTORY_H
#define CONTROL_FACTORY_H

#include "ControlBase.h"
#include "../Memory.h"
#include "SingleLED.h"
#include "DualLED.h"
#include "TripleLED.h"
#include "Button.h"
#include "Potentiometer.h"
#include "RotaryButton.h"

template <uint8 MaxControlCount, uint8 ProcessRate>
class ControlFactory
{
public:
	ControlFactory(IHAL *HAL)
		: m_HAL(HAL),
		  m_UsedGPIOs{},
		  m_Controls{},
		  m_ControlCount(0)
	{
	}

	SingleLED *CreateSingleLED(uint8 Pin)
	{
		MarkGPIOAsUsed(Pin);

		return Create<SingleLED>(m_HAL, Pin, ProcessRate);
	}
	DualLED *CreateDualLED(uint8 RedPin, uint8 GreenPin)
	{
		MarkGPIOAsUsed(RedPin);
		MarkGPIOAsUsed(GreenPin);

		return Create<DualLED>(m_HAL, RedPin, GreenPin, ProcessRate);
	}
	TripleLED *CreateTripleLED(uint8 RedPin, uint8 GreenPin, uint8 BluePin)
	{
		MarkGPIOAsUsed(RedPin);
		MarkGPIOAsUsed(GreenPin);
		MarkGPIOAsUsed(BluePin);

		return Create<TripleLED>(m_HAL, RedPin, GreenPin, BluePin, ProcessRate);
	}

	Button *CreateButton(uint8 Pin)
	{
		MarkGPIOAsUsed(Pin);

		return Create<Button>(m_HAL, Pin, ProcessRate);
	}

	Switch *CreateSwitch(uint8 Pin)
	{
		MarkGPIOAsUsed(Pin);

		return Create<Switch>(m_HAL, Pin, ProcessRate);
	}

	Potentiometer *CreatePotentiometer(uint8 Pin, bool FilterSwings = false)
	{
		MarkGPIOAsUsed(Pin);

		return Create<Potentiometer>(m_HAL, Pin, ProcessRate, FilterSwings);
	}

	Rotary *CreateRotary(uint8 LeftPin, uint8 RightPin)
	{
		MarkGPIOAsUsed(LeftPin);
		MarkGPIOAsUsed(RightPin);

		return Create<Rotary>(m_HAL, LeftPin, RightPin, ProcessRate);
	}

	RotaryButton *CreateRotaryButton(uint8 LeftPin, uint8 RightPin, uint8 ButtonPin)
	{
		MarkGPIOAsUsed(LeftPin);
		MarkGPIOAsUsed(RightPin);
		MarkGPIOAsUsed(ButtonPin);

		return Create<RotaryButton>(m_HAL, LeftPin, RightPin, ButtonPin, ProcessRate);
	}

	void Process(void)
	{
		for (uint8 i = 0; i < m_ControlCount; ++i)
			m_Controls[i]->Process();
	}

private:
	template <typename T, typename... ArgsT>
	T *Create(ArgsT... Args)
	{
		T *control = Memory::Allocate<T>();
		new (control) T(Args...);

		m_Controls[m_ControlCount++] = control;

		return control;
	}

	void MarkGPIOAsUsed(uint8 Pin)
	{
		ASSERT(!m_UsedGPIOs[Pin], "GPIOPin%i is already in use", Pin);

		m_UsedGPIOs[Pin] = true;
	}

private:
	IHAL *m_HAL;
	bool m_UsedGPIOs[MaxControlCount];
	ControlBase *m_Controls[MaxControlCount];
	uint8 m_ControlCount;
};

#endif