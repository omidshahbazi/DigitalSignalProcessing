#pragma once
#ifndef Math_H
#define Math_H

#include <cmath>

class Math
{
public:
	static constexpr double PI_VALUE = 3.14159265;
	static constexpr double HALF_PI_VALUE = PI_VALUE / 2;
	static constexpr double TWO_PI_VALUE = 2 * PI_VALUE;
	static constexpr double EPSILON = 0.0001F;
	static constexpr double TO_RADIANS = PI_VALUE / 180;
	static constexpr double TO_DEGREES = 180 / PI_VALUE;

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

	template <typename T>
	static T Ceil(T Value)
	{
		return ceil(Value);
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
	static T Wrap(T Value, U Min, V Max)
	{
		T rangeSize = Max - Min + 1;

		if (Value < Min)
			Value += rangeSize * ((Min - Value) / rangeSize + 1);

		return Min + (Value - Min) % rangeSize;
	}

	template <typename T, typename U, typename V, typename W>
	static T Map(T Value, T OldMin, U OldMax, V NewMin, W NewMax)
	{
		return (Value - OldMin) / (OldMax - OldMin) * (NewMax - NewMin) + NewMin;
	}

	template <typename T, typename U, typename V>
	static T Lerp(T Min, U Max, V Time)
	{
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

		Time = Clamp01(Time);

		return (Min * (1 - Time)) + (Max * Time);
	}

	template <typename T>
	static T Fraction(T Value)
	{
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

		return Value - (int32)Value;
	}

	template <typename T>
	static T Cube(T Value)
	{
		return (Value * Value) * Value;
	}

	template <typename T>
	static T Sin(T Value)
	{
		return (T)sin(Value);
	}

	template <typename T>
	static T Cos(T Value)
	{
		return (T)cos(Value);
	}

	template <typename T>
	static T Log2(T Value)
	{
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

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
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

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
	static T Power(T Value, int32 N)
	{
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

		long *lp, l;
		lp = (long *)(&Value);
		l = *lp;
		l -= 0x3F800000;
		l <<= (N - 1);
		l += 0x3F800000;
		*lp = l;
		return Value;
	}

	template <typename T>
	static T Root(T Value, int32 N)
	{
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

		long *lp, l;
		lp = (long *)(&Value);
		l = *lp;
		l -= 0x3F800000;
		l >>= (N - 1);
		l += 0x3F800000;
		*lp = l;
		return Value;
	}

	template <typename T>
	static T SoftClip(T Value)
	{
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

		if (Value < -1)
			return -1;
		else if (Value > 1)
			return 1;

		return Clamp(atan(Value), -1, 1);
	}

	// Factor: [8, 10000]
	template <typename T, typename U>
	static T HardClip(T Value, U Factor)
	{
		static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");

		//\arctan\left(\sqrt{1\ -\ \left(\sin x\right)^{3}}+\left(\left(f\ +\left(-\operatorname{sign}\left(f\right)\cdot10\right)\right)+\left(f\cdot\sin x\right)\right)\right)\cdot0.63

		// return atan(Root(1 - Power(Value, 3), 2) + Factor + (-Sign(Factor) * 10) + (Factor * Value)) * 0.63;
		return atan(Root(1 - Power(Value, 3), 2));
	}
};

#endif