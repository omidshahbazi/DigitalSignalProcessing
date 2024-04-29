#pragma once
#ifndef Math_H
#define Math_H

#include <cmath>
#include <type_traits>

class Math
{
public:
	template <typename T>
	static T Absolute(T Value)
	{
		return fabs(Value);
	}

	template <typename T, typename U>
	static T Moderate(T X, U Y)
	{
		return fmod(X, Y);
	}

	template <typename T>
	static int8 Sign(T Value)
	{
		return (0 < Value) - (Value < 0);
	}

	template <typename T, typename U>
	static T Min(T A, U B)
	{
		return (A < B ? A : B);
	}

	template <typename T, typename U>
	static T Max(T A, U B)
	{
		return (A > B ? A : B);
	}

	template <typename T, typename U, typename V>
	static T Clamp(T Value, U Min, V Max)
	{
		if (Value < Min)
			return Min;

		if (Value > Max)
			return Max;

		return Value;
	}

	template <typename T>
	static T Clamp01(T Value)
	{
		return Clamp(Value, 0, 1);
	}

	template <typename T>
	static T ClampExcluded0To1(T Value)
	{
		return Clamp(Value, EPSILON, 1);
	}

	template <typename T, typename U, typename V>
	static T Lerp(T Min, U Max, V Time)
	{
		static_assert(std::is_same<T, float>() || std::is_same<T, double>(), "T must be float or double");

		Time = Clamp01(Time);

		return (Min * (1 - Time)) + (Max * Time);
	}

	template <typename T>
	static T Fraction(T Value)
	{
		static_assert(std::is_same<T, float>() || std::is_same<T, double>(), "T must be float or double");

		return Value - (int32)Value;
	}

	template <typename T>
	static T Cube(T Value)
	{
		return (Value * Value) * Value;
	}

	// TODO: Needs to be tested
	template <typename T>
	static T Sawtooth(T Value)
	{
		static_assert(std::is_same<T, float>() || std::is_same<T, double>(), "T must be float or double");

		return 2 * (Value - floor(0.5 - Value));
	}

	template <typename T>
	static T Log2(T Value)
	{
		static_assert(std::is_same<T, float>() || std::is_same<T, double>(), "T must be float or double");

		T frac;
		int32 exp;
		frac = frexpf(Absolute(Value), &exp);
		Value = 1.23149591368684;
		Value *= frac;
		Value += -4.11852516267426;
		Value *= frac;
		Value += 6.02197014179219;
		Value *= frac;
		Value += -3.13396450166353;
		Value += exp;
		return Value;
	}

	template <typename T>
	static T Log10(T Value)
	{
		static_assert(std::is_same<T, float>() || std::is_same<T, double>(), "T must be float or double");

		return Log2(Value) * 0.3010299956639812;
	}

	template <typename T>
	static T Exponential(T Value)
	{
		return expf(Value);
	}

	template <typename T>
	static T Power10(T Value)
	{
		return Exponential(2.302585092994046 * Value);
	}

	template <typename T>
	static T SoftClip(T Value)
	{
		static_assert(std::is_same<T, float>() || std::is_same<T, double>(), "T must be float or double");

		if (Value < -1)
			return -1;
		else if (Value > 1)
			return 1;

		return atan(Value);
	}

public:
	static constexpr double PI_VALUE = 3.14159265;
	static constexpr double HALF_PI_VALUE = PI_VALUE / 2;
	static constexpr double TWO_PI_VALUE = 2 * PI_VALUE;
	static constexpr double EPSILON = 0.0001F;
};

#endif