#pragma once

#include <inttypes.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef unsigned int uint32;
typedef uint64_t uint64;

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