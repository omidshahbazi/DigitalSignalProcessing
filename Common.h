#pragma once

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef const char *cstr;

#define KHz 1000
#define MHz 1000 * KHz

#define SAMPLE_RATE_8000 8 * KHz
#define SAMPLE_RATE_15750 15.75 * KHz
#define SAMPLE_RATE_16000 16 * KHz
#define SAMPLE_RATE_22050 22.05 * KHz
#define SAMPLE_RATE_24000 24 * KHz
#define SAMPLE_RATE_32000 32 * KHz
#define SAMPLE_RATE_44100 44.1 * KHz
#define SAMPLE_RATE_48000 48 * KHz
#define SAMPLE_RATE_96000 96 * KHz
#define SAMPLE_RATE_192000 192 * KHz
#define SAMPLE_RATE_320000 320 * KHz

#define MIN_SAMPLE_RATE SAMPLE_RATE_8000
#define MAX_SAMPLE_RATE SAMPLE_RATE_320000

#define MIN_FREQUENCY 20.0
#define MAX_FREQUENCY 20.0 * KHz

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

public:
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;
};

uint16 GetStringLength(cstr Value)
{
	if (Value == nullptr)
		return 0;

	uint16 len = 0;
	while (Value[len] != '\0')
		++len;

	return len;
}

#define ASSERT_ON_FLOATING_TYPE(T) static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double")
#define ASSERT_ON_SAMPLE_RATE(SampleRate) static_assert(MIN_SAMPLE_RATE <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate")