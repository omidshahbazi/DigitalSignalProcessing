#pragma once
#ifndef FINITE_IMPULSE_RESPONSE_FILTER_H
#define FINITE_IMPULSE_RESPONSE_FILTER_H

#include "Filter.h"
#include "../Memory.h"

#ifdef ARM_SIMD_FINITE_IMPULSE_RESPONSE
#include "arm_math.h"
#endif

template <typename T, uint32 SampleRate, uint8 FrameLength, uint16 MaxTaps>
class FiniteImpulseResponseFilter : public Filter<T, SampleRate>
{
#ifdef ARM_SIMD_FINITE_IMPULSE_RESPONSE
	ASSERT_ON_ONLY_FLOAT_TYPE(T);
	static_assert(MaxTaps <= 2048, "FIR filter cannot handle more than 2048 IR taps");
#else
	static_assert(MaxTaps <= 1024, "FIR filter cannot handle more than 1024 IR taps");
#endif

public:
	FiniteImpulseResponseFilter(void)
#ifdef ARM_SIMD_FINITE_IMPULSE_RESPONSE
		: m_State(nullptr)
#else
		: m_Coefficients(nullptr),
		  m_TapCount(0),
		  m_DelayLine(nullptr),
		  m_WriteIndex(0)
#endif
	{
#ifdef ARM_SIMD_FINITE_IMPULSE_RESPONSE
		const uint16 StateCount = MaxTaps + FrameLength - 1;
		m_State = Memory::Allocate<T>(StateCount, true);

		arm_fir_init_f32(&m_Instance, 0, nullptr, m_State, FrameLength);
#else
		m_DelayLine = Memory::Allocate<T>(MaxTaps, true);
#endif
	}

	void SetData(const T *Data, uint16 Length)
	{
		ASSERT(Length <= MaxTaps, "Invalid Length %u", Length);

		if (Data == nullptr || Length == 0)
		{
			static const T SingleTap = 1;
			Data = &SingleTap;
			Length = 1;
		}

#ifdef ARM_SIMD_FINITE_IMPULSE_RESPONSE
		arm_fir_init_f32(&m_Instance, Length, Data, m_State, FrameLength);
#else
		m_Coefficients = Data;
		m_TapCount = Length;
#endif
	}

	void Process(T *Buffer, uint8 Count) override
	{
#ifdef ARM_SIMD_FINITE_IMPULSE_RESPONSE
		arm_fir_f32(&m_Instance, Buffer, Buffer, Count);
#else
		if (m_Coefficients == nullptr)
			return;

		for (uint8 n = 0; n < Count; n++)
		{
			m_DelayLine[m_WriteIndex] = Buffer[n];

			T output = 0;

			uint16 readIndex = m_WriteIndex;
			for (uint16 i = 0; i < m_TapCount; i++)
			{
				output += m_Coefficients[i] * m_DelayLine[readIndex];

				if (readIndex == 0)
					readIndex = MaxTaps - 1;
				else
					readIndex--;
			}

			if (++m_WriteIndex >= MaxTaps)
				m_WriteIndex = 0;

			Buffer[n] = output;
		}
#endif
	}

private:
#ifdef ARM_SIMD_FINITE_IMPULSE_RESPONSE
	arm_fir_instance_f32 m_Instance;
	T *m_State;
#else
	const T *m_Coefficients;
	uint16 m_TapCount;

	T *m_DelayLine;
	uint16 m_WriteIndex;
#endif
};

#endif