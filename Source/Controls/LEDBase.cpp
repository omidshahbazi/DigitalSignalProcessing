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

void LEDBase::SetConstantBrightness(cc_ratio_t Value)
{
	m_DefaultValue = Value;

	SetBrightnessFunction([Value](void) { return Value; });
}

void LEDBase::SetBlinkingBrightness(cc_ratio_t MinValue, cc_ratio_t MaxValue, pfloat Rate)
{
	m_DefaultMinValue = MinValue;
	m_DefaultMaxValue = MaxValue;
	m_DefaultRate = Rate;

	SetBrightnessFunction([this](void)
		{
			float value = Math::Moderate(m_HAL->GetTimeSinceStartup() * 2 * m_DefaultRate, 2);

			return ((int32_t)value == 0 ? m_DefaultMinValue : m_DefaultMaxValue);
		});
}

void LEDBase::SetFadingBrightness(cc_ratio_t MinValue, cc_ratio_t MaxValue, pfloat Rate)
{
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