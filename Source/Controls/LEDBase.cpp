#include "DigitalSignalProcessing/Controls/LEDBase.h"

LEDBase::LEDBase(IHAL* HAL)
	: m_HAL(HAL),
	m_DefaultValue(0),
	m_DefaultMinValue(0),
	m_DefaultMaxValue(0),
	m_DefaultRate(0)
{
	SetConstantBrightness(0);
}

void LEDBase::SetConstantBrightness(float Value)
{
	ASSERT(0 <= Value && Value <= 1, "Invalid Value %f", Value);

	m_DefaultValue = Value;

	SetBrightnessFunction([Value](void) { return Value; });
}

void LEDBase::SetBlinkingBrightness(float MinValue, float MaxValue, float Rate)
{
	ASSERT(0 <= MinValue && MinValue <= 1, "Invalid MinValue %f", MinValue);
	ASSERT(0 <= MaxValue && MaxValue <= 1, "Invalid MaxValue %f", MaxValue);
	ASSERT(0 < Rate, "Invalid Rate %f", Rate);

	m_DefaultMinValue = MinValue;
	m_DefaultMaxValue = MaxValue;
	m_DefaultRate = Rate;

	SetBrightnessFunction([this](void)
		{
			float value = Math::Moderate(m_HAL->GetTimeSinceStartup() * 2 * m_DefaultRate, 2);

			return ((int32_t)value == 0 ? m_DefaultMinValue : m_DefaultMaxValue);
		});
}

void LEDBase::SetFadingBrightness(float MinValue, float MaxValue, float Rate)
{
	ASSERT(0 <= MinValue && MinValue <= 1, "Invalid MinValue %f", MinValue);
	ASSERT(0 <= MaxValue && MaxValue <= 1, "Invalid MaxValue %f", MaxValue);
	ASSERT(0 < Rate, "Invalid Rate %f", Rate);

	m_DefaultMinValue = MinValue;
	m_DefaultMaxValue = MaxValue;
	m_DefaultRate = Rate;

	SetBrightnessFunction([this](void)
		{
			float value = Math::Moderate(m_HAL->GetTimeSinceStartup() * 2 * m_DefaultRate, 2);
			if (value > 1)
				value = 1 - (value - 1);

			return Math::Lerp(m_DefaultMinValue, m_DefaultMaxValue, value);
		});
}