#pragma once
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "DataTypes.h"

uint16 GetStringLength(cstr Value);

cstr IntToString(int32 Value);

cstr FloatToString(float Value, int Decimals = 1);

template <typename T>
static cstr ToString(T Value)
{
	return IntToString(Value);
}

template <>
cstr ToString<float>(float Value)
{
	return FloatToString(Value);
}

uint8 GetNoteIndex(uint8 MIDINumber);

cstr GetEnglishNoteName(uint8 MIDINumber);

cstr GetFrenchNoteName(uint8 MIDINumber);

#endif