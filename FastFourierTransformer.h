#pragma once
#ifndef FAST_FOURIER_TRANSFORMER_H
#define FAST_FOURIER_TRANSFORMER_H

#include "Common.h"
#include "Math.h"
#include "Debug.h"

#ifdef ARM_SIMD_FAST_FOURIER_TRANSFORMER
#include "arm_math.h"
#endif

class FastFourierTransformer
{
public:
	class Complex
	{
	public:
		Complex(float Real = 0, float Imaginary = 0)
			: Real(Real),
			  Imaginary(Imaginary)
		{
		}

		float Magnitude(void) const
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

	public:
		float Real;
		float Imaginary;
	};

public:
	template <uint16 SampleCount>
	static void FastFourierTransform(Complex *Buffer)
	{
		static_assert(Math::IsPowerOfTwo(SampleCount), "SampleCount must be power-of-two");

#ifdef ARM_SIMD_FAST_FOURIER_TRANSFORMER
		arm_cfft_instance_f32 S;
		arm_cfft_init_f32(&S, SampleCount);
		arm_cfft_f32(&S, reinterpret_cast<float *>(Buffer), 0, 1);
#else
		for (uint16 i = 1, j = 0; i < SampleCount; i++)
		{
			uint16 bit = SampleCount >> 1;
			for (; j & bit; bit >>= 1)
				j ^= bit;
			j ^= bit;

			if (i < j)
			{
				Complex temp = Buffer[i];
				Buffer[i] = Buffer[j];
				Buffer[j] = temp;
			}
		}

		for (uint16 len = 2; len <= SampleCount; len <<= 1)
		{
			float ang = -Math::TWO_PI_VALUE / (float)len;
			Complex wlen(Math::Cos(ang), Math::Sin(ang));
			for (uint16 i = 0; i < SampleCount; i += len)
			{
				Complex w(1, 0);
				for (uint16 j = 0; j < len / 2; j++)
				{
					Complex u = Buffer[i + j];
					Complex v = Buffer[i + j + len / 2] * w;
					Buffer[i + j] = u + v;
					Buffer[i + j + len / 2] = u - v;
					w = w * wlen;
				}
			}
		}
#endif
	}

	template <uint32 SampleRate, uint16 SampleCount>
	static float CalculateFrequency(Complex *Buffer, float Threshold = 0.05)
	{
		FastFourierTransform<SampleCount>(Buffer);

		const uint16 HALF_LEN = SampleCount / 2;
		float magnitudes[HALF_LEN];

#ifdef ARM_SIMD_FAST_FOURIER_TRANSFORMER
		arm_cmplx_mag_f32(reinterpret_cast<float *>(Buffer), magnitudes, HALF_LEN);
#else
		for (uint16 i = 0; i < HALF_LEN; ++i)
			magnitudes[i] = Buffer[i].Magnitude();
#endif

		float maxValue = 0;

#ifdef ARM_SIMD_FAST_FOURIER_TRANSFORMER
		uint32_t maxIdx = 0;
		arm_max_f32(magnitudes + 1, HALF_LEN - 1, &maxValue, &maxIdx);
		maxIdx += 1;
#else
		uint16 maxIdx = 1;
		maxValue = magnitudes[1];
		for (uint16 i = 2; i < HALF_LEN; ++i)
		{
			if (magnitudes[i] > maxValue)
			{
				maxValue = magnitudes[i];
				maxIdx = i;
			}
		}
#endif

		if (maxValue < Threshold)
			return 0;

		float fineIndex = (float)maxIdx;
		if (maxIdx > 0 && maxIdx < HALF_LEN - 1)
		{
			float a = magnitudes[maxIdx - 1];
			float b = magnitudes[maxIdx];
			float c = magnitudes[maxIdx + 1];
			// Precision boost: find the true peak between FFT bins
			float denominator = (a - 2.0f * b + c);
			if (Math::Absolute(denominator) > Math::EPSILON)
				fineIndex += 0.5f * (a - c) / denominator;
		}

		return fineIndex * (float)SampleRate / (float)SampleCount;
	}

	template <typename T, uint32 SampleRate, uint16 SampleCount>
	static float CalculateFrequencyAligned(const T *const AlignedBuffer, float Threshold = 0.05)
	{
		Complex buffer[SampleCount];
		for (int i = 0; i < SampleCount; ++i)
			buffer[i] = Complex(Math::HannWindow<T, SampleCount>(AlignedBuffer[i], i), 0.0f);

		return CalculateFrequency<SampleRate, SampleCount>(buffer, Threshold);
	}
	
	template <typename T, uint32 SampleRate, uint16 SampleCount>
	static float CalculateFrequencyRaw(const T *const RignBuffer, uint16 CurrentIndex, float Threshold = 0.05)
	{
		T alignedBuffer[SampleCount];
		for (uint16 i = 0; i < SampleCount; ++i)
			alignedBuffer[i] = RignBuffer[(CurrentIndex + i) & (SampleCount - 1)];

		return CalculateFrequencyAligned<T, SampleRate, SampleCount>(alignedBuffer, Threshold);
	}
};

#endif