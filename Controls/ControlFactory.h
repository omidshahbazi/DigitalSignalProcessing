#pragma once
#ifndef CONTROL_FACTORY_H
#define CONTROL_FACTORY_H

#include "Control.h"
#include "../Memory.h"
#include "../Time.h"
#include <vector>
#include <algorithm>

class ControlFactory
{
public:
	template <typename T, typename... ArgsT>
	T *Create(ArgsT... Args)
	{
		T *control = Memory::Allocate<T>(1, true);
		new (control) T(Args...);

		m_Controls.push_back(control);

		return control;
	}

	template <typename T>
	void Destroy(T *Control)
	{
		m_Controls.erase(std::find(m_Controls.begin, m_Controls.end, Control));

		Memory::Deallocate(Control);
	}

	void Process(void)
	{
		for (Control *control : m_Controls)
			control->Process();
	}

private:
	std::vector<Control *> m_Controls;
};

#endif