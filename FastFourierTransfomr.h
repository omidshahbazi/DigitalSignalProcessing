#pragma once
#ifndef FAST_FOURIER_TRANSFOMER_H
#define FAST_FOURIER_TRANSFOMER_H

#include "Common.h"
#include "Math.h"
#include "Debug.h"

class FastFourierTransfomr
{
public:
	class Complex
	{
	public:
		double Real;
		double Imaginary;

		Complex(double Real = 0, double Imaginary = 0)
			: Real(Real),
			  Imaginary(Imaginary)
		{
		}

		double Magnitude(void) const
		{
			return std::sqrt(Real * Real + Imaginary * Imaginary);
		}

		Complex operator+(const Complex &Other) const
		{
			return Complex(Real + Other.Real, Imaginary + Other.Imaginary);
		}

		Complex operator-(const Complex &Other) const
		{
			return Complex(Real - Other.Real, Imaginary - Other.Imaginary);
		}

		Complex operator*(const Complex &Other) const
		{
			return Complex(Real * Other.Real - Imaginary * Other.Imaginary, Real * Other.Imaginary + Imaginary * Other.Real);
		}
	};

public:
	template <uint16 SampleCount>
	static void FastFourierTransform(Complex *Buffer)
	{
		static_assert(Math::IsPowerOfTwo(SampleCount), "SampleCount must be power-of-two");

		FastFourierTransformRecursive<SampleCount>(Buffer);
	}

	template <uint32 SampleRate, uint16 SampleCount>
	static float CalculateFrequency(Complex *Buffer)
	{
		FastFourierTransform<SampleCount>(Buffer);

		const uint16 HALF_LEN = SampleCount / 2;
		double magnitudes[HALF_LEN];
		for (int i = 0; i < HALF_LEN; ++i)
			magnitudes[i] = Buffer[i].Magnitude();

		uint16 index = FindMaxIndex(magnitudes, SampleCount);

		return index * SampleRate / (double)SampleCount;
	}

	template <typename T, uint32 SampleRate, uint16 SampleCount>
	static float CalculateFrequency(const T *const Buffer)
	{
		Complex buffer[SampleCount];
		for (int i = 0; i < SampleCount; ++i)
			buffer[i] = Buffer[i];

		return CalculateFrequency<SampleRate, SampleCount>(buffer);
	}

private:
	template <uint16 SampleCount>
	static void FastFourierTransformRecursive(Complex *Buffer)
	{
		if (SampleCount <= 1)
			return;

		ASSERT(Buffer != nullptr, "Invalid Buffer");

		const uint16 HALF_LEN = SampleCount / 2;

		// Divide
		Complex even[HALF_LEN];
		Complex odd[HALF_LEN];
		for (uint16 i = 0; i < HALF_LEN; ++i)
		{
			even[i] = Buffer[i * 2];
			odd[i] = Buffer[i * 2 + 1];
		}

		// Conquer
		FastFourierTransformRecursive<HALF_LEN>(even);
		FastFourierTransformRecursive<HALF_LEN>(odd);

		// Combine
		for (uint16 i = 0; i < HALF_LEN; ++i)
		{
			Complex t = Complex(Math::Cos(-Math::TWO_PI_VALUE * i / (double)SampleCount), Math::Sin(-Math::TWO_PI_VALUE * i / (double)SampleCount)) * odd[i];

			Buffer[i] = even[i] + t;
			Buffer[i + HALF_LEN] = even[i] - t;
		}
	}

	template <typename T>
	static uint16 FindMaxIndex(const T *const Buffer, uint16 Length)
	{
		ASSERT(Buffer != nullptr, "Invalid Buffer");
		ASSERT(Length > 0, "Invalid Length");

		uint16 maxIndex = 0;
		T maxValue = Buffer[0];

		for (uint16 i = 1; i < Length; ++i)
		{
			if (Buffer[i] <= maxValue)
				continue;

			maxValue = Buffer[i];
			maxIndex = i;
		}

		return maxIndex;
	}
};

#endif