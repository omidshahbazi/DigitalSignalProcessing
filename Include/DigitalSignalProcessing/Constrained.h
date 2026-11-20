#pragma once
#ifndef CONSTRAINED_H
#define CONSTRAINED_H

#include "DataTypes.h"
#include "Debug.h"

typedef int64_t ConstraintValue_t;

template<ConstraintValue_t V>
struct Greater
{
	static constexpr ConstraintValue_t Value = V;
	static constexpr const char* Operator = ">";

	template<typename T>
	static constexpr bool Check(T X)
	{
		static_assert(static_cast<T>(V) == V, "Constraint value not representable in T");

		return X > static_cast<T>(V);
	}
};

template<ConstraintValue_t V>
struct GreaterEqual
{
	static constexpr ConstraintValue_t Value = V;
	static constexpr const char* Operator = ">=";

	template<typename T>
	static constexpr bool Check(T X)
	{
		static_assert(static_cast<T>(V) == V, "Constraint value not representable in T");

		return X >= static_cast<T>(V);
	}
};

template<ConstraintValue_t V>
struct Less
{
	static constexpr ConstraintValue_t Value = V;
	static constexpr const char* Operator = "<";

	template<typename T>
	static constexpr bool Check(T X)
	{
		static_assert(static_cast<T>(V) == V, "Constraint value not representable in T");

		return X < static_cast<T>(V);
	}
};

template<ConstraintValue_t V>
struct LessEqual
{
	static constexpr ConstraintValue_t Value = V;
	static constexpr const char* Operator = "<=";

	template<typename T>
	static constexpr bool Check(T X)
	{
		static_assert(static_cast<T>(V) == V, "Constraint value not representable in T");

		return X <= static_cast<T>(V);
	}
};

template<ConstraintValue_t V>
struct NotEqual
{
	static constexpr ConstraintValue_t Value = V;
	static constexpr const char* Operator = "!=";

	template<typename T>
	static constexpr bool Check(T X)
	{
		static_assert(static_cast<T>(V) == V, "Constraint value not representable in T");

		return X != static_cast<T>(V);
	}
};

template<typename T, typename... Constraints>
class Constrained
{
	static_assert(sizeof...(Constraints) > 0, "At least one constraint is required");

public:
	Constrained(void) = delete;   // explicit value required (see below)

	Constrained(T Value)
		: m_Value{ Validate(Value) }
	{}

	template<typename... OtherConstraints>
	Constrained(Constrained<T, OtherConstraints...> Other)
		: m_Value{ Validate(static_cast<T>(Other)) }
	{}

	Constrained& operator=(T Value)
	{
		m_Value = Validate(Value);
		return *this;
	}

	template<typename... OtherConstraints>
	Constrained& operator=(Constrained<T, OtherConstraints...> Other)
	{
		m_Value = Validate(static_cast<T>(Other));
		return *this;
	}

	operator T(void) const
	{
		return m_Value;
	}

private:
	template<typename Constraint>
	static void Validate(T Value)
	{
		ASSERT(Constraint::Check(Value), "Expected [%f] %s [%f]", static_cast<double>(Value), Constraint::Operator, static_cast<long long>(Constraint::Value));
	}

	static T Validate(T Value)
	{
		(Validate<Constraints>(Value), ...);

		return Value;
	}

	T m_Value;
};

template<typename T>
using non_zero_value = Constrained<T, NotEqual<0>>;

template<typename T>
using positive_value = Constrained<T, Greater<0>>;

template<typename T, ConstraintValue_t Min, ConstraintValue_t Max>
using cc_range_value = Constrained<T, GreaterEqual<Min>, LessEqual<Max>>;

template<typename T, ConstraintValue_t Min, ConstraintValue_t Max>
using oc_range_value = Constrained<T, Greater<Min>, LessEqual<Max>>;

template<typename T, ConstraintValue_t Min, ConstraintValue_t Max>
using co_range_value = Constrained<T, GreaterEqual<Min>, Less<Max>>;

template<typename T, ConstraintValue_t Min, ConstraintValue_t Max>
using oo_range_value = Constrained<T, Greater<Min>, Less<Max>>;

#endif