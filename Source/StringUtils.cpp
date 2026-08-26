#include "DigitalSignalProcessing/StringUtils.h"
#include "DigitalSignalProcessing/Notes.h"
#include "DigitalSignalProcessing/Math.h"
#include <stdio.h>

uint16 GetStringLength(cstr Value)
{
	if (Value == nullptr)
		return 0;

	uint16 len = 0;
	while (Value[len] != '\0')
		++len;

	return len;
}

cstr IntToString(int32 Value)
{
	static char buffer[8];

	snprintf(buffer, sizeof(buffer), "%i", Value);

	return buffer;
}

cstr FloatToString(float Value, int Decimals)
{
	static char buffer[16];

	int idx = 0;

	if (Decimals < 0)
		Decimals = 0;
	if (Decimals > 4)
		Decimals = 4;

	if (Value < 0)
	{
		buffer[idx++] = '-';
		Value = -Value;
	}

	static const int pow10[] = { 1, 10, 100, 1000, 10000 };
	int mult = pow10[Decimals];

	int scaled = (int)roundf(Value * mult);
	int int_part = scaled / mult;
	int frac_part = scaled % mult;

	int len = snprintf(buffer + idx, sizeof(buffer) - idx, "%d", int_part);
	if (len < 0)
		return buffer;

	idx += len;

	if (Decimals > 0 && frac_part > 0)
	{
		int active_decimals = Decimals;
		while (active_decimals > 0 && frac_part % 10 == 0)
		{
			frac_part /= 10;
			active_decimals--;
		}

		if (active_decimals > 0)
		{
			buffer[idx++] = '.';

			char temp[5];
			for (int i = 0; i < active_decimals; i++)
			{
				temp[active_decimals - 1 - i] = '0' + (frac_part % 10);
				frac_part /= 10;
			}

			for (int i = 0; i < active_decimals; i++)
				buffer[idx++] = temp[i];
		}
	}

	buffer[idx] = '\0';

	return buffer;
}

uint8 GetNoteIndex(uint8 MIDINumber)
{
	return Math::Moderate(MIDINumber, TotalNoteCount);
}

cstr GetEnglishNoteName(uint8 MIDINumber)
{
	return EnglishNoteNames[GetNoteIndex(MIDINumber)];
}

cstr GetFrenchNoteName(uint8 MIDINumber)
{
	return FrenchNoteNames[Math::Moderate(MIDINumber, TotalNoteCount)];
}