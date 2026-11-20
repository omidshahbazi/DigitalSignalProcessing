#pragma once
#ifndef EXTENDED_DATA_TYPES_H
#define EXTENDED_DATA_TYPES_H

#include "DataTypes.h"
#include "Constrained.h"
#include "Gain.h"
#include "Frequency.h"

#define SPECIFIC_TYPES() \
	HANDLE_TYPE(dBGain) \
	HANDLE_TYPE(Frequency)

#define ALL_TYPES() \
	SIGNED_INTEGER_TYPES() \
	UNSIGNED_INTEGER_TYPES() \
	FLOATING_POINT_TYPES() \
	SPECIFIC_TYPES()


#define HANDLE_TYPE(Type) using nz##Type = non_zero_value<Type>;
SIGNED_INTEGER_TYPES();
FLOATING_POINT_TYPES();
#undef HANDLE_TYPE


#define HANDLE_TYPE(Type) using p##Type = positive_value<Type>;
UNSIGNED_INTEGER_TYPES();
FLOATING_POINT_TYPES();
#undef HANDLE_TYPE


#define HANDLE_TYPE(Type) \
	template<ConstraintValue_t Min, ConstraintValue_t Max> \
	using cc_##Type = cc_range_value<Type, Min, Max>; \
	template<ConstraintValue_t Min, ConstraintValue_t Max> \
	using oc_##Type = oc_range_value<Type, Min, Max>; \
	template<ConstraintValue_t Min, ConstraintValue_t Max> \
	using co_##Type = co_range_value<Type, Min, Max>; \
	template<ConstraintValue_t Min, ConstraintValue_t Max> \
	using oo_##Type = oo_range_value<Type, Min, Max>;
ALL_TYPES();
#undef HANDLE_TYPE


#define HANDLE_TYPE(Type, Name) \
	typedef cc_range_value<Type, 0, 1> cc_##Name##_t; \
	typedef oc_range_value<Type, 0, 1> oc_##Name##_t; \
	typedef co_range_value<Type, 0, 1> co_##Name##_t; \
	typedef oo_range_value<Type, 0, 1> oo_##Name##_t;
HANDLE_TYPE(float, ratio);
#undef HANDLE_TYPE


typedef cc_dBGain<-12, 12> dBGain_12;
typedef cc_dBGain<-20, 20> dBGain_20;


typedef oc_Frequency<0, MAX_FREQUENCY_VALUE> Frequency_full;
typedef cc_Frequency<MIN_FREQUENCY_VALUE, MAX_FREQUENCY_VALUE> Frequency_hearable;

#endif