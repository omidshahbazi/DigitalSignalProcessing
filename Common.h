#pragma once

#include <stdio.h>
#include "DataTypes.h"
#include "Gain.h"
#include "Octave.h"
#include "Notes.h"

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef const char *cstr;

#define KB *1024
#define MB *1048576

#define KHz *1'000

static constexpr uint32 SAMPLE_RATE_8000 = 8 KHz;
static constexpr uint32 SAMPLE_RATE_15750 = 15.75 KHz;
static constexpr uint32 SAMPLE_RATE_16000 = 16 KHz;
static constexpr uint32 SAMPLE_RATE_22050 = 22.05 KHz;
static constexpr uint32 SAMPLE_RATE_24000 = 24 KHz;
static constexpr uint32 SAMPLE_RATE_32000 = 32 KHz;
static constexpr uint32 SAMPLE_RATE_44100 = 44.1 KHz;
static constexpr uint32 SAMPLE_RATE_48000 = 48 KHz;
static constexpr uint32 SAMPLE_RATE_96000 = 96 KHz;
static constexpr uint32 SAMPLE_RATE_192000 = 192 KHz;
static constexpr uint32 SAMPLE_RATE_320000 = 320 KHz;

static constexpr uint32 MIN_SAMPLE_RATE = SAMPLE_RATE_8000;
static constexpr uint32 MAX_SAMPLE_RATE = SAMPLE_RATE_320000;

static constexpr float MIN_FREQUENCY = 5.0;
static constexpr float MAX_FREQUENCY = 20.0 KHz;

#define ms *0.001
#define ns *0.000'001

// https://www.redcrab-software.com/en/Calculator/Electrics/Decibel-Factor
static const dBGain MIN_GAIN(-90);
static const dBGain SILENCE_GAIN(-40);
static const dBGain NORMAL_GAIN(0);
static const dBGain MAX_GAIN(6);

static const Octave ONE_OCTAVE(-1);
static const Octave ONE_OCTAVE_UP(-1);
static const Octave ONE_OCTAVE_DOWN(1);
static const Octave HALF_OCTAVE_UP(-0.5);
static const Octave HALF_OCTAVE_DOWN(0.5);

#define SWAP_ENDIAN_16BIT(Value) ((Value >> 8) | ((Value & 0xFF) << 8))

#ifdef DEBUG
#ifdef ENABLE_TYPE_CHECK
#include <type_traits>

#define ARE_TYPES_THE_SAME(A, B) std::is_same<A, B>()
#else
#define ARE_TYPES_THE_SAME(A, B) true
#endif
#else
#define ARE_TYPES_THE_SAME(A, B) true
#endif

struct Color
{
public:
	Color(void)
		: R(0),
		  G(0),
		  B(0),
		  A(255)
	{
	}

	Color(uint8 R, uint8 G, uint8 B)
		: R(R),
		  G(G),
		  B(B),
		  A(255)
	{
	}

	Color(uint8 R, uint8 G, uint8 B, uint8 A)
		: R(R),
		  G(G),
		  B(B),
		  A(A)
	{
	}

	template <typename T>
	Color operator*(T Value) const
	{
		Color col = *this;
		col *= Value;
		return col;
	}

	template <typename T>
	Color &operator*=(T Value)
	{
		R = CombineValues(R, Value);
		G = CombineValues(G, Value);
		B = CombineValues(B, Value);

		return *this;
	}

	uint16 R5G6B5(void)
	{
		uint16 b = (B >> 3) & 0x1f;
		uint16 g = ((G >> 2) & 0x3f) << 5;
		uint16 r = ((R >> 3) & 0x1f) << 11;

		return (uint16)(r | g | b);
	}

	static uint16 BlendR5G6B5(uint16 ColorA, uint16 ColorB, uint8 Alpha)
	{
		//   rrrrrggggggbbbbb
		const uint16 MASK_RB = 63519;		// 0b1111100000011111
		const uint16 MASK_G = 2016;			// 0b0000011111100000
		const uint32 MASK_MUL_RB = 4065216; // 0b1111100000011111000000
		const uint32 MASK_MUL_G = 129024;	// 0b0000011111100000000000
		const uint16 MAX_ALPHA = 64;		// 6bits+1 with rounding

		// alpha for foreground multiplication
		// convert from 8bit to (6bit+1) with rounding
		// will be in [0..64] inclusive
		Alpha = (Alpha + 2) >> 2;
		// "beta" for background multiplication; (6bit+1);
		// will be in [0..64] inclusive
		uint8 beta = MAX_ALPHA - Alpha;
		// so (0..64)*alpha + (0..64)*beta always in 0..64

		return (uint16)((((Alpha * (uint32)(ColorA & MASK_RB) + beta * (uint32)(ColorB & MASK_RB)) & MASK_MUL_RB) | ((Alpha * (ColorA & MASK_G) + beta * (ColorB & MASK_G)) & MASK_MUL_G)) >> 6);
	}

	static uint8 CombineValues(uint8 A, uint8 B)
	{
		// return (uint8)(255 * (A / 255.F) * (B / 255.F));
		return (uint8)((A * B) / 255.F);
	}

public:
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;
};

static const Color ColorBlack = {0, 0, 0, 255};
static const Color ColorWhite = {255, 255, 255, 255};

static const Color ColorDarkGray = {100, 100, 100, 255};
static const Color ColorGray = {127, 127, 127, 255};
static const Color ColorLightGray = {186, 186, 186, 255};

static const Color ColorDarkRed = {100, 0, 0, 255};
static const Color ColorRed = {186, 0, 0, 255};
static const Color ColorLightRed = {255, 0, 0, 255};

static const Color ColorDarkGreen = {0, 100, 0, 255};
static const Color ColorGreen = {0, 186, 0, 255};
static const Color ColorLightGreen = {0, 255, 0, 255};

static const Color ColorBlue = {0, 0, 186, 255};
static const Color ColorLightBlue = {0, 0, 255, 255};

static uint16 GetStringLength(cstr Value)
{
	if (Value == nullptr)
		return 0;

	uint16 len = 0;
	while (Value[len] != '\0')
		++len;

	return len;
}

static cstr IntToString(int32 Value)
{
	static char buffer[8];

	snprintf(buffer, sizeof(buffer), "%i", Value);

	return buffer;
}

static cstr FloatToString(float Value)
{
	static char buffer[8];

	uint32 length = snprintf(buffer, sizeof(buffer), "%.1f", Value);

	if (length >= 2 && buffer[length - 2] == '.' && buffer[length - 1] == '0')
		buffer[length - 2] = '\0';

	return buffer;
}

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

static uint8 GetNoteIndex(uint8 MIDINumber)
{
	return Math::Moderate(MIDINumber, TotalNoteCount);
}

static cstr GetEnglishNoteName(uint8 MIDINumber)
{
	return EnglishNoteNames[GetNoteIndex(MIDINumber)];
}

static cstr GetFrenchNoteName(uint8 MIDINumber)
{
	return FrenchNoteNames[Math::Moderate(MIDINumber, TotalNoteCount)];
}

#ifdef ENABLE_TYPE_CHECK
#define ASSERT_ON_SAMPLE_RATE(SampleRate) static_assert(MIN_SAMPLE_RATE <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate")
#else
#define ASSERT_ON_SAMPLE_RATE(SampleRate)
#endif

#ifdef MAX_FRAME_LENGTH
static_assert(MAX_FRAME_LENGTH > 0, "Invalid MAX_FRAME_LENGTH defined");
#else
#define MAX_FRAME_LENGTH 64
#endif

#ifdef STANDARD_UP_SAMPLE_FACTOR
static_assert(STANDARD_UP_SAMPLE_FACTOR > 1, "Invalid STANDARD_UP_SAMPLE_FACTOR defined");
#else
#define STANDARD_UP_SAMPLE_FACTOR 2
#endif

#define CLONE_BUFFER(Name)                                                                                      \
	ASSERT(MAX_FRAME_LENGTH >= Count, "Insofficient buffer size for " #Name " %i<%i", MAX_FRAME_LENGTH, Count); \
	static T Name[MAX_FRAME_LENGTH];                                                                            \
	Memory::Copy(Buffer, Name, Count);

#define CREATE_UP_SAMPLE_BUFFER(Name, FactorValue) \
	const uint8 Name##Factor = FactorValue;        \
	T Name[MAX_FRAME_LENGTH * FactorValue];        \
	const uint8 Name##Length = Count * FactorValue;

#define CREATE_STANDARD_UP_SAMPLE_BUFFER(Name) CREATE_UP_SAMPLE_BUFFER(Name, STANDARD_UP_SAMPLE_FACTOR)