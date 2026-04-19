#pragma once
#ifndef Math_H
#define Math_H

#include <cmath>
#include <limits>
#include <algorithm>
#include "DataTypes.h"
#include "Notes.h"

class Math
{
public:
	static constexpr double PI_VALUE = 3.14159265;
	static constexpr double HALF_PI_VALUE = PI_VALUE / 2;
	static constexpr double TWO_PI_VALUE = 2 * PI_VALUE;
	static constexpr double EPSILON = std::numeric_limits<float>::epsilon();
	static constexpr double TO_RADIANS = PI_VALUE / 180;
	static constexpr double TO_DEGREES = 180 / PI_VALUE;
	static constexpr double LOG_NATURAL_2 = 0.69314718;
	static constexpr double HALF_LOG_NATURAL_2 = LOG_NATURAL_2 * 0.5;

#ifdef FAST_MATH
private:
	class LookupTable
	{
	private:
		static constexpr uint16 Size = 512;
		static constexpr float SinScale = (Size - 1) / TWO_PI_VALUE;
		static constexpr float TanHRange = 3.0;
		static constexpr float TanHScale = (Size - 1) / (2.0 * TanHRange);

	public:
		LookupTable(void)
		{
			for (uint16 i = 0; i < Size; i++)
			{
				m_SinLUT[i] = std::sin((TWO_PI_VALUE * i) / (Size - 1));
				m_TanHLUT[i] = std::tanh(-TanHRange + (2 * TanHRange * i) / (Size - 1));
				m_LogLUT[i] = std::log(1.0f + (float)i / (Size - 1));
			}
		}

		template <typename T>
		inline T Sin(T Value) const
		{
			ASSERT_ON_FLOATING_TYPE(T);

			Value = std::fmod(Value, TWO_PI_VALUE);
			if (Value < 0)
				Value += TWO_PI_VALUE;

			float pos = Value * SinScale;
			uint16 index = static_cast<uint16>(pos);
			uint16 next = (index + 1) % Size;
			float frac = pos - index;

			return m_SinLUT[index] + (frac * (m_SinLUT[next] - m_SinLUT[index]));
		}

		template <typename T>
		inline T Cos(T Value) const
		{
			return Sin(Value + HALF_PI_VALUE);
		}

		template <typename T>
		inline T TanH(T Value) const
		{
			ASSERT_ON_FLOATING_TYPE(T);

			if (Value <= -TanHRange)
				return -1;
			if (Value >= TanHRange)
				return 1;

			float pos = (Value + TanHRange) * TanHScale;
			uint16 index = static_cast<uint16>(pos);
			uint16 next = (index + 1) % Size;
			float frac = pos - index;

			return m_TanHLUT[index] + (frac * (m_TanHLUT[next] - m_TanHLUT[index]));
		}

		template <typename T>
		inline T Log(T Value) const
		{
			ASSERT_ON_FLOATING_TYPE(T);

			if (Value <= 0)
				return -std::numeric_limits<T>::infinity();

			int n;
			float f = std::frexp((float)Value, &n);

			f *= 2.0f;
			n -= 1;

			float pos = (f - 1.0f) * (Size - 1);
			uint16 index = static_cast<uint16>(pos);
			float frac = pos - index;

			float result = m_LogLUT[index] + (frac * (m_LogLUT[index + 1] - m_LogLUT[index]));

			return (T)(result + (n * LOG_NATURAL_2));
		}

		template <typename T>
		inline T ASinh(T Value) const
		{
			return Log(Value + Math::SquareRoot(Value * Value + 1));
		}

	private:
		float m_SinLUT[Size];
		float m_TanHLUT[Size];
		float m_LogLUT[Size];
	};

	static const LookupTable &GetLUT(void)
	{
		static LookupTable lut;
		return lut;
	}
#endif

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

	template <typename T>
	static int32 Round(T A, float Threshold)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		T absA = Absolute(A);

		if (absA - (int32)absA > Threshold)
			return (int32)A + Sign(A);

		return (int32)A;
	}

	template <typename T, typename U>
	static auto Min(T A, U B) -> decltype(A * B)
	{
		return (A < B ? A : B);
	}

	template <typename T, typename U>
	static auto Max(T A, U B) -> decltype(A * B)
	{
		return (A > B ? A : B);
	}

	template <typename T>
	static T Ceil(T Value)
	{
		return ceil(Value);
	}

	template <typename T, typename U, typename V>
	static auto Clamp(T Value, U Min, V Max) -> decltype(Value * Min * Max)
	{
		if (Value < Min)
			return Min;

		if (Value > Max)
			return Max;

		return Value;
	}

	template <typename T>
	static auto Clamp01(T Value)
	{
		return Clamp(Value, 0, 1);
	}

	template <typename T>
	static auto ClampSignal(T Value)
	{
		return Clamp(Value, -1, 1);
	}

	template <typename T>
	static auto ClampExcluded0To1(T Value)
	{
		return Clamp(Value, EPSILON, 1);
	}

	template <typename T, typename U, typename V>
	static auto Wrap(T Value, U Min, V Max) -> decltype(Value * Min * Max)
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
	static auto Map(T Value, T OldMin, U OldMax, V NewMin, W NewMax) -> decltype(NewMin * NewMax)
	{
		return (Value - OldMin) / (float)(OldMax - OldMin) * (float)(NewMax - NewMin) + NewMin;
	}

	template <typename T, typename U, typename V, typename W>
	static auto MapLinearToLogarithmic(T Value, T OldMin, U OldMax, V NewMin, W NewMax) -> decltype(NewMin * NewMax)
	{
		float newMinLog = Log2(NewMin);
		float newMaxLog = Log2(NewMax);

		return Clamp(Power2(Map(Value, (float)OldMin, (float)OldMax, newMinLog, newMaxLog)), NewMin, NewMax);
	}

	template <typename T, typename U, typename V, typename W>
	static auto MapLogarithmicToLinear(T Value, T OldMin, U OldMax, V NewMin, W NewMax) -> decltype(NewMin * NewMax)
	{
		float oldMinLog = Log2(OldMin);
		float oldMaxLog = Log2(OldMax);

		return Map((float)Log2(Value), oldMinLog, oldMaxLog, (float)NewMin, (float)NewMax);
	}

	template <typename T, typename U, typename V>
	static auto Lerp(T Min, U Max, V Time) -> decltype(Min * Max * Time)
	{
		ASSERT_ON_FLOATING_TYPE(V);

		Time = Clamp01(Time);

		return (Min * (1 - Time)) + (Max * Time);
	}

	template <typename T, typename U, typename V>
	static auto FrequencyLerp(T Min, U Max, V Time) -> decltype(Min * Max * Time)
	{
		ASSERT_ON_FLOATING_TYPE(V);

		return Min * Power(Max / Min, Time);
	}

	template <typename T, typename U>
	static auto FrequencyDiff(T Min, U Max) -> decltype(Min * Max)
	{
		return Log2(Max / Min);
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
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		return (T)GetLUT().Sin(Value);
#else
		return (T)sin(Value);
#endif
	}

	template <typename T>
	static T SinH(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return (Exponential(Value) - Exponential(-Value)) / 2;
	}

	template <typename T>
	static T Cos(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		return (T)GetLUT().Cos(Value);
#else
		return (T)cos(Value);
#endif
	}

	template <typename T>
	static T TanH(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		return (T)GetLUT().TanH(Value);
#else
		return (T)tanh(Value);
#endif
	}

	template <typename T>
	static T ASinh(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		return (T)GetLUT().ASinh(Value);
#else
		return (T)std::asinh(Value);
#endif
	}

	template <typename T>
	static T Log(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		return (T)GetLUT().Log(Value);
#else
		return std::log(Value);
#endif
	}

	template <typename T>
	static T Log2(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		// union
		// {
		// 	float f;
		// 	uint32 i;
		// } vx = {(float)Value};
		// union
		// {
		// 	uint32 i;
		// 	float f;
		// } mx;

		// float exp = (float)((vx.i >> 23) & 0xFF) - 127;

		// mx.i = (vx.i & 0x007FFFFF) | 0x3f800000;

		// float y = mx.f;
		// float log_m = -0.34484843f * y * y + 2.02466578f * y - 1.67487566f;

		// return (T)(exp + log_m);

		return Log(Value) * (T)1.4426950408889634;
#else
		return (T)log2f(Value);
#endif
	}

	template <typename T>
	static T Log10(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		return Log2(Value) * 0.3010299956639812;
#else
		return (T)log10f(Value);
#endif
	}

	template <typename T>
	static T Exponential(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		return (T)Power2(Value * 1.4426950408);
#else
		return AccurateExponential(Value);
#endif
	}

	template <typename T>
	static T AccurateExponential(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return (T)std::exp(Value);
	}

	template <typename T, typename U>
	static auto Power(T Value, U N) -> decltype(Value * N)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return (T)std::pow(Value, N);
	}

	template <typename T>
	static T Power2(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

#ifdef FAST_MATH
		float clipp = Value < -126.0f ? -126.0f : Value;
		union
		{
			uint32 i;
			float f;
		} v;

		int i = (int)clipp;
		float f = clipp - i;
		if (clipp < 0)
		{
			i--;
			f++;
		}

		v.i = (uint32)((i + 127) << 23);
		v.f *= (1.0f + 0.69314718f * f + 0.24022650f * f * f);

		return (T)v.f;
#else
		return (T)Power(2.0, Value);
#endif
	}

	template <typename T>
	static constexpr bool IsPowerOfTwo(T Value)
	{
		return (Value != 0) && ((Value & (Value - 1)) == 0);
	}

	template <typename T>
	static T SquareRoot(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return (T)(std::sqrt(Value));
	}

	// Amount [-1, 1]
	template <typename T>
	static float GetAsymmetricGain(T Value, float Amount)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		const float PositiveScale = 1 + Amount;
		const float NegativeScale = 1 - Amount;

		return (Value > 0 ? PositiveScale : NegativeScale);
	}

	// Asymmetry [-1, 1]
	template <typename T>
	static T SoftClip(T Value, float Asymmetry = 0)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		const float SideFactor = GetAsymmetricGain(Value, Asymmetry);

		return TanH(Value * SideFactor);
	}

	// Factor [0, 1)
	// Asymmetry [-1, 1]
	template <typename T>
	static T CrunchClip(T Value, float Factor = 0, float Asymmetry = 0)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		const float SideFactor = Clamp(Factor * GetAsymmetricGain(Value, Asymmetry), 0, 0.99);

		return Value / ((1 - SideFactor) + Absolute(Value));
	}

	template <typename T>
	static T AgressiveClip(T Value, float Asymmetry = 0)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		const float SideFactor = GetAsymmetricGain(Value, Asymmetry);

		return Value / (1 + Absolute(Value * SideFactor));
	}

	template <typename T>
	static T HarshClip(T Value, float Asymmetry = 0)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		const float SideFactor = GetAsymmetricGain(Value, Asymmetry);

		return (Value > 0 ? 1 : -1) * (1 - Exponential(-Absolute(Value * SideFactor)));
	}

	// Factor: [0, 1)
	// Asymmetry [-1, 1]
	template <typename T>
	static T HardClip(T Value, float Factor = 0, float Asymmetry = 0)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		const float BottomThreshold = -Factor * GetAsymmetricGain(1.0, Asymmetry);
		const float TopThreshold = Factor * GetAsymmetricGain(-1.0, Asymmetry);

		return Clamp(Value, BottomThreshold, TopThreshold);
	}

	template <typename T>
	static T dBToLinear(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return (T)Power(10.0, Value / 20);
	}

	template <typename T>
	static T LinearTodB(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return (T)(20 * Log10(Value + EPSILON));
	}

	template <typename T>
	static T OctaveToLinear(T Value)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return (T)Power(2, Value);
	}

	template <typename T>
	static T AdditiveMix(T Dry, T Wet, float WetRatio = 1)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return Dry + (Wet * WetRatio);
	}

	template <typename T>
	static T MultiplicativeMix(T A, T B)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return A * B;
	}

	template <typename T>
	static T LinearCrossFadeMix(T A, T B, float Ratio)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		return Lerp(A, B, Ratio);
	}

	template <typename T>
	static T ConstantPowerCrossFadeMix(T Dry, T Wet, float WetRatio)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		const float angle = WetRatio * HALF_PI_VALUE;
		const T gainA = cosf(angle);
		const T gainB = sinf(angle);

		return (Dry * gainA) + (Wet * gainB);
	}

	// The integer MIDI note (440 -> 69 (A4))
	// The deviation in cents +/-0.5
	template <typename T>
	static T FrequencyToMidi(T Value, T A4Frequencey = NOTE_A4)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		if (Value <= 0)
			return 0;

		return Clamp(12 * Log2(Value / A4Frequencey) + 69, 0, 128);
	}

	template <typename T>
	static void UpSample(const T *Input, uint16 Count, T *Output, uint8 Ratio)
	{
		const float invRatio = 1.0 / Ratio;

		for (uint16 i = 0; i < Count - 1; ++i)
		{
			T current = Input[i];
			T next = Input[i + 1];

			for (uint16 j = 0; j < Ratio; ++j)
			{
				float fraction = j * invRatio;

				Output[i * Ratio + j] = Lerp(current, next, fraction);
			}
		}

		// Handle the last sample manually
		for (uint32 j = 0; j < Ratio; ++j)
			Output[(Count - 1) * Ratio + j] = Input[Count - 1];
	}

	template <typename T>
	static void DownSample(const T *Input, uint16 Count, T *Output, uint8 Ratio)
	{
		uint16 outputIndex = 0;

		for (uint16 i = 0; i < Count; i += Ratio)
			Output[outputIndex++] = Input[i];
	}

	template <typename T>
	static T GetMeanValue(T *Buffer, uint16 Count)
	{
		if (Buffer == nullptr)
			return 0;

		if (Count == 0)
			return 0;

		T *middle = Buffer + (Count / 2);

		std::nth_element(Buffer, middle, Buffer + Count);

		return *middle;
	}

	template <typename T, uint16 SampleCount>
	static T HannWindow(T Value, uint16 Index)
	{
		ASSERT_ON_FLOATING_TYPE(T);

		float window = 0.5 * (1 - Cos(TWO_PI_VALUE * Index / (SampleCount - 1)));

		return Value * window;
	}

	template <typename T, uint16 SampleCount>
	static void HannWindow(T *Buffer, uint16 Count)
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] *= HannWindow(Buffer[i], i);
	}
};

#endif