#pragma once
#ifndef CONTROL_FACTORY_H
#define CONTROL_FACTORY_H

#include "ControlBase.h"
#include "../Memory.h"

class ControlFactory
{
private:
	static const uint8 MAX_CONTROL_COUNT = 31;

public:
	ControlFactory(void)
		: m_ControlCount(0)
	{
	}

	template <typename T, typename... ArgsT>
	T *Create(ArgsT... Args)
	{
		T *control = Memory::Allocate<T>();
		new (control) T(Args...);

		m_Controls[m_ControlCount++] = control;

		return control;
	}

	void Process(void)
	{
		for (uint8 i = 0; i < m_ControlCount; ++i)
			m_Controls[i]->Process();
	}

private:
	ControlBase *m_Controls[MAX_CONTROL_COUNT];
	uint8 m_ControlCount;
};

#endif