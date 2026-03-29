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