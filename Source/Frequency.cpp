#include "DigitalSignalProcessing/Frequency.h"
#include "DigitalSignalProcessing/Math.h"

void FixBandFrequency(Frequency& Low, Frequency& High)
{
	// LowerBoundRatio = e^(2 * ASinH(1/20))
	const double LowerBoundRatio = 1.1053423;
	const double UpperBoundRatio = 10000;

	High = (Frequency)Math::Clamp(High, Low * LowerBoundRatio, Low * UpperBoundRatio);

	if (High > HIGHEST_FREQUENCY)
	{
		High = HIGHEST_FREQUENCY;

		Low = (Frequency)(High / LowerBoundRatio);
	}
}

LinearOctave::LinearOctave(Frequency Low, Frequency High)
	: m_Value(0)
{
	FixBandFrequency(Low, High);

	m_Value = High / Low;
}

LinearOctave::LinearOctave(LogarithmicOctave Value)
	: m_Value(Math::Power2((float)Value))
{}


LogarithmicOctave::LogarithmicOctave(LinearOctave Value)
	: m_Value(Math::Log2((float)Value))
{}

LogarithmicOctave::LogarithmicOctave(Frequency Low, Frequency High)
	: m_Value((float)(LogarithmicOctave)LinearOctave(Low, High))
{}

Frequency operator*(Frequency Frequency, LinearOctave Octave)
{
	return ::Frequency((float)Frequency * (float)Octave);
}

Frequency operator/(Frequency Frequency, LinearOctave Octave)
{
	return ::Frequency((float)Frequency / (float)Octave);
}

Frequency operator*(Frequency Frequency, LogarithmicOctave Octave)
{
	return Frequency * LinearOctave(Octave);
}

Frequency operator/(Frequency Frequency, LogarithmicOctave Octave)
{
	return Frequency / LinearOctave(Octave);
}

FrequencyBand::FrequencyBand(Frequency Center, QualityFactor Quality)
	: m_Center(Center)
{
	SetQualityFactor(Quality);
}

FrequencyBand::FrequencyBand(Frequency Low, Frequency High)
{
	FixBandFrequency(Low, High);

	m_Center = (Frequency)Math::FrequencyLerp(Low, High, 0.5);
	m_Bandwidth = LogarithmicOctave(Low, High);
}

void FrequencyBand::SetQualityFactor(QualityFactor Quality)
{
	m_Bandwidth = LogarithmicOctave(2 * Math::ASinh(1 / (2 * Quality)) / Math::LOG_NATURAL_2);
}

QualityFactor FrequencyBand::GetQualityFactor(void) const
{
	return QualityFactor(*this);
}

FrequencyBand::operator QualityFactor(void) const
{
	return GetQualityFactor();
}

Frequency FrequencyBand::GetLow(void) const
{
	return (Frequency)Math::Max(LOWEST_FREQUENCY, m_Center / LinearOctave(LogarithmicOctave(m_Bandwidth * 0.5)));
}

Frequency FrequencyBand::GetHigh(void) const
{
	return (Frequency)Math::Min(HIGHEST_FREQUENCY, m_Center * LinearOctave(LogarithmicOctave(m_Bandwidth * 0.5)));
}

QualityFactor::QualityFactor(FrequencyBand Band)
{
	m_Value = 1 / (2 * Math::SinH(Math::HALF_LOG_NATURAL_2 * Band.GetBandwidth()));
}