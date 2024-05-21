#pragma once

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

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

public:
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;
};