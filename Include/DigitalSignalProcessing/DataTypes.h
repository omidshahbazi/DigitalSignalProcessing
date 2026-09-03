#pragma once

#ifdef __GNUC__
#include <stdint.h>
#else
#include <cstdint>
#endif

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

#if defined(__GNUC__)
#define STRINGIFY_PRAGMA(x) _Pragma(#x)
#define BEGIN_PACK(n)       STRINGIFY_PRAGMA(pack(push, n))
#define END_PACK()          STRINGIFY_PRAGMA(pack(pop))
#else
#define BEGIN_PACK(n) __pragma(pack(push, n))
#define END_PACK()    __pragma(pack(pop))
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

BEGIN_PACK(1);
struct uint24_t
{
private:
	uint8_t m_Bytes[3];

public:
	uint24_t(void) = default;

	uint24_t(uint32_t Value)
	{
		*this = Value;
	}

	uint24_t& operator=(uint32_t Value)
	{
		m_Bytes[0] = static_cast<uint8_t>(Value & 0xFF);
		m_Bytes[1] = static_cast<uint8_t>((Value >> 8) & 0xFF);
		m_Bytes[2] = static_cast<uint8_t>((Value >> 16) & 0xFF);

		return *this;
	}

	operator uint32_t() const
	{
		return static_cast<uint32_t>(m_Bytes[0]) |
			(static_cast<uint32_t>(m_Bytes[1]) << 8) |
			(static_cast<uint32_t>(m_Bytes[2]) << 16);
	}
};
END_PACK();

BEGIN_PACK(1);
struct int24_t
{
private:
	uint8_t m_Bytes[3];

public:
	int24_t(void) = default;

	int24_t(int32_t Value)
	{
		*this = Value;
	}

	int24_t& operator=(int32_t Value)
	{
		m_Bytes[0] = static_cast<uint8_t>(Value & 0xFF);
		m_Bytes[1] = static_cast<uint8_t>((Value >> 8) & 0xFF);
		m_Bytes[2] = static_cast<uint8_t>((Value >> 16) & 0xFF);

		return *this;
	}

	operator int32_t(void) const
	{
		uint32_t value = static_cast<uint32_t>(m_Bytes[0]) |
			(static_cast<uint32_t>(m_Bytes[1]) << 8) |
			(static_cast<uint32_t>(m_Bytes[2]) << 16);

		if (value & 0x00800000)
			value |= 0xFF000000;

		return static_cast<int32_t>(value);
	}
};
END_PACK();