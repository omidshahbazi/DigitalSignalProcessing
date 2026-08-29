#pragma once

#include "DataTypes.h"

#if defined(DEBUG)
#define TURN_OFF_OPTIMIZER() \
 		_Pragma("GCC push_options") \
 		_Pragma("GCC optimize (\"O0\")")
#define TURN_ON_OPTIMIZER() _Pragma("GCC pop_options")
#elif defined(RELEASE)
#define TURN_OFF_OPTIMIZER()
#define TURN_ON_OPTIMIZER()
#else
#error "Neither of DEBUG or RELASE are defined"
#endif

static constexpr uint32 SAMPLE_RATE_8000 = 8'000;
static constexpr uint32 SAMPLE_RATE_15750 = 15'750;
static constexpr uint32 SAMPLE_RATE_16000 = 16'000;
static constexpr uint32 SAMPLE_RATE_22050 = 220'500;
static constexpr uint32 SAMPLE_RATE_24000 = 24'000;
static constexpr uint32 SAMPLE_RATE_32000 = 32'000;
static constexpr uint32 SAMPLE_RATE_44100 = 44'100;
static constexpr uint32 SAMPLE_RATE_48000 = 48'000;
static constexpr uint32 SAMPLE_RATE_96000 = 96'000;
static constexpr uint32 SAMPLE_RATE_192000 = 192'000;
static constexpr uint32 SAMPLE_RATE_320000 = 320'000;
static constexpr uint8 SAMPLE_RATE_COUNT = 11;

static constexpr uint32 MIN_SAMPLE_RATE = SAMPLE_RATE_8000;
static constexpr uint32 MAX_SAMPLE_RATE = SAMPLE_RATE_320000;

static constexpr uint8 BIT_DEPTH_8 = 8;
static constexpr uint8 BIT_DEPTH_16 = 16;
static constexpr uint8 BIT_DEPTH_24 = 24;
static constexpr uint8 BIT_DEPTH_32 = 32;
static constexpr uint8 BIT_DEPTH_COUNT = 4;

#define ASSERT_ON_SAMPLE_RATE(SampleRate) static_assert(MIN_SAMPLE_RATE <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate")

#define KB *1024
#define MB *1048576

#define ns *0.000'001
#define ms *0.001

#define SWAP_ENDIAN_16BIT(Value) ((Value >> 8) | ((Value & 0xFF) << 8))

#ifndef MAX_FRAME_LENGTH
#define MAX_FRAME_LENGTH 64
#endif
static_assert(MAX_FRAME_LENGTH > 0, "Invalid MAX_FRAME_LENGTH defined");

#ifndef UP_SAMPLE_FACTOR
#define UP_SAMPLE_FACTOR 4
#endif
static_assert(UP_SAMPLE_FACTOR > 1, "Invalid UP_SAMPLE_FACTOR defined");

#define CLONE_BUFFER(Name)                                                                                      \
	ASSERT(MAX_FRAME_LENGTH >= Count, "Insufficient buffer size for " #Name " %i<%i", MAX_FRAME_LENGTH, Count); \
	static T Name[MAX_FRAME_LENGTH];                                                                            \
	Memory::Copy(Buffer, Name, Count);

#define CREATE_UP_SAMPLE_BUFFER(Name, FactorValue) \
	const uint8 Name##Factor = FactorValue;        \
	T Name[MAX_FRAME_LENGTH * FactorValue];        \
	const uint8 Name##Length = Count * FactorValue;

#define CREATE_STANDARD_UP_SAMPLE_BUFFER(Name) CREATE_UP_SAMPLE_BUFFER(Name, UP_SAMPLE_FACTOR)

#if defined(__GNUC__)
#define STRINGIFY_PRAGMA(x) _Pragma(#x)
#define BEGIN_PACK(n)       STRINGIFY_PRAGMA(pack(push, n))
#define END_PACK()          STRINGIFY_PRAGMA(pack(pop))
#else
#define BEGIN_PACK(n) __pragma(pack(push, n))
#define END_PACK()    __pragma(pack(pop))
#endif