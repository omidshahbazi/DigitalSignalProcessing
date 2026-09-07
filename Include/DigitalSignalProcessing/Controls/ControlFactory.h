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

template <uint8_t MaxControlCount, uint16_t ProcessRate>
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

	SingleLED *CreateSingleLED(uint8_t Pin)
	{
		MarkGPIOAsUsed(Pin);

		return Create<SingleLED>(m_HAL, Pin, ProcessRate);
	}
	DualLED *CreateDualLED(uint8_t RedPin, uint8_t GreenPin)
	{
		MarkGPIOAsUsed(RedPin);
		MarkGPIOAsUsed(GreenPin);

		return Create<DualLED>(m_HAL, RedPin, GreenPin, ProcessRate);
	}
	TripleLED *CreateTripleLED(uint8_t RedPin, uint8_t GreenPin, uint8_t BluePin)
	{
		MarkGPIOAsUsed(RedPin);
		MarkGPIOAsUsed(GreenPin);
		MarkGPIOAsUsed(BluePin);

		return Create<TripleLED>(m_HAL, RedPin, GreenPin, BluePin, ProcessRate);
	}

	Button *CreateButton(uint8_t Pin)
	{
		MarkGPIOAsUsed(Pin);

		return Create<Button>(m_HAL, Pin);
	}

	Switch *CreateSwitch(uint8_t Pin)
	{
		MarkGPIOAsUsed(Pin);

		return Create<Switch>(m_HAL, Pin);
	}

	Potentiometer *CreatePotentiometer(uint8_t Pin, bool FilterSwings = false)
	{
		MarkGPIOAsUsed(Pin);

		return Create<Potentiometer>(m_HAL, Pin, ProcessRate, FilterSwings);
	}

	Rotary *CreateRotary(uint8_t LeftPin, uint8_t RightPin)
	{
		MarkGPIOAsUsed(LeftPin);
		MarkGPIOAsUsed(RightPin);

		return Create<Rotary>(m_HAL, LeftPin, RightPin);
	}

	RotaryButton *CreateRotaryButton(uint8_t APin, uint8_t BPin, uint8_t ButtonPin)
	{
		MarkGPIOAsUsed(APin);
		MarkGPIOAsUsed(BPin);
		MarkGPIOAsUsed(ButtonPin);

		return Create<RotaryButton>(m_HAL, APin, BPin, ButtonPin);
	}

	void Process(void)
	{
		for (uint8_t i = 0; i < m_ControlCount; ++i)
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

	void MarkGPIOAsUsed(uint8_t Pin)
	{
		ASSERT(!m_UsedGPIOs[Pin], "GPIOPin%i is already in use", Pin);

		m_UsedGPIOs[Pin] = true;
	}

private:
	IHAL *m_HAL;
	bool m_UsedGPIOs[MaxControlCount];
	ControlBase *m_Controls[MaxControlCount];
	uint8_t m_ControlCount;
};

#endif