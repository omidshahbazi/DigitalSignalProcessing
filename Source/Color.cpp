#include "DigitalSignalProcessing/Color.h"

Color::Color(void)
	: R(0),
	G(0),
	B(0),
	A(255)
{}

Color::Color(uint8 R, uint8 G, uint8 B)
	: R(R),
	G(G),
	B(B),
	A(255)
{}

Color::Color(uint8 R, uint8 G, uint8 B, uint8 A)
	: R(R),
	G(G),
	B(B),
	A(A)
{}

uint16 Color::R5G6B5(void)
{
	uint16 b = (B >> 3) & 0x1f;
	uint16 g = ((G >> 2) & 0x3f) << 5;
	uint16 r = ((R >> 3) & 0x1f) << 11;

	return (uint16)(r | g | b);
}

uint16 Color::BlendR5G6B5(uint16 ColorA, uint16 ColorB, uint8 Alpha)
{
	//   rrrrrggggggbbbbb
	const uint16 MASK_RB = 63519;		// 0b1111100000011111
	const uint16 MASK_G = 2016;			// 0b0000011111100000
	const uint32 MASK_MUL_RB = 4065216; // 0b1111100000011111000000
	const uint32 MASK_MUL_G = 129024;	// 0b0000011111100000000000
	const uint16 MAX_ALPHA = 64;		// 6bits+1 with rounding

	// alpha for foreground multiplication
	// convert from 8bit to (6bit+1) with rounding
	// will be in [0..64] inclusive
	Alpha = (Alpha + 2) >> 2;
	// "beta" for background multiplication; (6bit+1);
	// will be in [0..64] inclusive
	uint8 beta = MAX_ALPHA - Alpha;
	// so (0..64)*alpha + (0..64)*beta always in 0..64

	return (uint16)((((Alpha * (uint32)(ColorA & MASK_RB) + beta * (uint32)(ColorB & MASK_RB)) & MASK_MUL_RB) | ((Alpha * (ColorA & MASK_G) + beta * (ColorB & MASK_G)) & MASK_MUL_G)) >> 6);
}

uint8 Color::CombineValues(uint8 A, uint8 B)
{
	// return (uint8)(255 * (A / 255.F) * (B / 255.F));
	return (uint8)((A * B) / 255.F);
}