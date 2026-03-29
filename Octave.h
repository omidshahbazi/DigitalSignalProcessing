#pragma once
#ifndef OCTAVE_H
#define OCTAVE_H

#include "Math.h"

struct Octave
{
public:
	Octave(void) = default;
	Octave(float Value)
	{
		m_Value = Math::OctaveToLinear(Value);
	}

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 1.0;
};

#endif