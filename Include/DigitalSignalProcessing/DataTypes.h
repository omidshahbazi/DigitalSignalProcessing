#pragma once

#ifdef __GNUC__
#include <stdint.h>
#else
#include <cstdint>
#endif

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef const char* cstr;

struct uint24
{
	uint24(void) = default;
	uint24(int32 Value)
		: full(Value)
	{}
	uint24(uint32 Value)
		: full(Value)
	{}
	uint24(const uint24& Other)
		: full(Other.full)
	{}

	operator uint32(void) const
	{
		return full;
	}

	union
	{
	public:
		uint32 full;
		uint8 bytes[3];
	};
};

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

#ifdef ENABLE_TYPE_CHECK
#define ASSERT_ON_ONLY_FLOAT_TYPE(T) static_assert(ARE_TYPES_THE_SAME(T, float), "T must be float")
#define ASSERT_ON_FLOATING_TYPE(T) static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double")
#define ASSERT_ON_NOT_FLOATING_TYPE(T) static_assert(!ARE_TYPES_THE_SAME(T, float) && !ARE_TYPES_THE_SAME(T, double), "T must not be float or double")
#else
#define ASSERT_ON_ONLY_FLOAT_TYPE(T)
#define ASSERT_ON_FLOATING_TYPE(T)
#define ASSERT_ON_NOT_FLOATING_TYPE(T)
#endif