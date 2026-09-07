#include "DigitalSignalProcessing/Color.h"

Color::Color(void)
	: R(0),
	G(0),
	B(0),
	A(255)
{}

Color::Color(uint8_t R, uint8_t G, uint8_t B)
	: R(R),
	G(G),
	B(B),
	A(255)
{}

Color::Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
	: R(R),
	G(G),
	B(B),
	A(A)
{}

uint16_t Color::R5G6B5(void)
{
	uint16_t b = (B >> 3) & 0x1f;
	uint16_t g = ((G >> 2) & 0x3f) << 5;
	uint16_t r = ((R >> 3) & 0x1f) << 11;

	return (uint16_t)(r | g | b);
}

uint16_t Color::BlendR5G6B5(uint16_t ColorA, uint16_t ColorB, uint8_t Alpha)
{
	//   rrrrrggggggbbbbb
	const uint16_t MASK_RB = 63519;		// 0b1111100000011111
	const uint16_t MASK_G = 2016;			// 0b0000011111100000
	const uint32_t MASK_MUL_RB = 4065216; // 0b1111100000011111000000
	const uint32_t MASK_MUL_G = 129024;	// 0b0000011111100000000000
	const uint16_t MAX_ALPHA = 64;		// 6bits+1 with rounding

	// alpha for foreground multiplication
	// convert from 8bit to (6bit+1) with rounding
	// will be in [0..64] inclusive
	Alpha = (Alpha + 2) >> 2;
	// "beta" for background multiplication; (6bit+1);
	// will be in [0..64] inclusive
	uint8_t beta = MAX_ALPHA - Alpha;
	// so (0..64)*alpha + (0..64)*beta always in 0..64

	return (uint16_t)((((Alpha * (uint32_t)(ColorA & MASK_RB) + beta * (uint32_t)(ColorB & MASK_RB)) & MASK_MUL_RB) | ((Alpha * (ColorA & MASK_G) + beta * (ColorB & MASK_G)) & MASK_MUL_G)) >> 6);
}

uint8_t Color::CombineValues(uint8_t A, uint8_t B)
{
	// return (uint8_t)(255 * (A / 255.F) * (B / 255.F));
	return (uint8_t)((A * B) / 255.F);
}