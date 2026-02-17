#pragma once
#ifndef Math_H
#define Math_H

#include <cmath>
#include <limits>
#include "Common.h"

class Math
{
public:
	static constexpr double PI_VALUE = 3.14159265;
	static constexpr double HALF_PI_VALUE = PI_VALUE / 2;
	static constexpr double TWO_PI_VALUE = 2 * PI_VALUE;
	static constexpr double EPSILON = std::numeric_limits<float>::epsilon();
	static constexpr double TO_RADIANS = PI_VALUE / 180;
	static constexpr double TO_DEGREES = 180 / PI_VALUE;

public:
	template <typename T>
	static bool IsNAN(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return std::isnan(Value);
	}

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
		ASSERT_ON_NOT_FLOATING_TYPE(T);
		ASSERT_ON_NOT_FLOATING_TYPE(U);
		ASSERT_ON_NOT_FLOATING_TYPE(V);

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
		ASSERT_ON_FLOATING_TYPE(T);

		Time = Clamp01(Time);

		return (Min * (1 - Time)) + (Max * Time);
	}

	template <typename T>
	static T Fraction(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

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
	static T Log2(T x) 
	{
		union { float f; uint32_t i; } vx = { (float)x };
		union { uint32_t i; float f; } mx;
		
		float exp = (float)((vx.i >> 23) & 0xFF) - 127;
		
		mx.i = (vx.i & 0x007FFFFF) | 0x3f800000;
		
		float y = mx.f;
		float log_m = -0.34484843f * y * y + 2.02466578f * y - 1.67487566f;

		return exp + log_m;
	}

	template <typename T>
	static T Log10(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return Log2(Value) * 0.3010299956639812;
	}

	template <typename T>
	static T Exponential(T Value)
	{
		// return expf(Value);

		Value = 1 + Value / 1024.0;

		Value *= Value;
		Value *= Value;
		Value *= Value;
		Value *= Value;
		Value *= Value;
		Value *= Value;
		Value *= Value;
		Value *= Value;
		Value *= Value;
		Value *= Value;

		return Value;
	}

	template <typename T>
	static T Power(T Value, int32 N)
	{
		return std::pow(Value, N);

		// long *lp, l;
		// lp = (long *)(&Value);
		// l = *lp;
		// l -= 0x3F800000;
		// l <<= (N - 1);
		// l += 0x3F800000;
		// *lp = l;
		// return Value;
	}

	template <typename T>
	static constexpr bool IsPowerOfTwo(T Value)
	{
		return (Value != 0) && ((Value & (Value - 1)) == 0);
	}

	template <typename T>
	static T SquareRoot(T Value)
	{
		return std::sqrt(Value);
	}

	template <typename T>
	static T SoftClip(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return tanh(Value);
	}

	// Factor: [8, 10000]
	template <typename T, typename U>
	static T HardClip(T Value, U Factor)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		//\arctan\left(\sqrt{1\ -\ \left(\sin x\right)^{3}}+\left(\left(f\ +\left(-\operatorname{sign}\left(f\right)\cdot10\right)\right)+\left(f\cdot\sin x\right)\right)\right)\cdot0.63

		// return atan(Root(1 - Power(Value, 3), 2) + Factor + (-Sign(Factor) * 10) + (Factor * Value)) * 0.63;
		return atan(SquareRoot(1 - Power(Value, 3)));
	}

	template <typename T>
	static T dBToLinear(T dB)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return Power(10, dB / 20);
	}

	template <typename T>
	static T LinearTodB(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return 20 * Log10(Value + EPSILON);
	}

	template <typename T, typename U, typename V>
	static T FrequencyLerp(T Min, U Max, V Time)
	{
		return Min * Power(Max / Min, Time);
	}
};

#endif