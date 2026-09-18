#include "DigitalSignalProcessing/Common.h"

uint32_t CRC32(const uint8_t* Data, uint32_t Size)
{
	uint32_t crc = 0xFFFFFFFFu;

	for (uint32_t i = 0; i < Size; ++i)
	{
		crc ^= Data[i];

		for (int b = 0; b < 8; ++b)
		{
			uint32_t mask = -(crc & 1u);
			crc = (crc >> 1) ^ (0xEDB88320u & mask);
		}
	}
	return ~crc;
}