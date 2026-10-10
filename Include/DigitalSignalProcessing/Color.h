#pragma once
#ifndef COLOR_H
#define COLOR_H

#include "DataTypes.h"

struct Color
{
public:
	constexpr Color(void)
		: R(0),
		G(0),
		B(0),
		A(255)
	{}

	constexpr Color(uint8_t R, uint8_t G, uint8_t B)
		: R(R),
		G(G),
		B(B),
		A(255)
	{}

	constexpr Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
		: R(R),
		G(G),
		B(B),
		A(A)
	{}

	template <typename T>
	Color operator*(T Value) const
	{
		Color col = *this;
		col *= Value;
		return col;
	}

	template <typename T>
	Color& operator*=(T Value)
	{
		R = CombineValues(R, Value);
		G = CombineValues(G, Value);
		B = CombineValues(B, Value);

		return *this;
	}

	uint16_t R5G6B5(void);

	static uint16_t BlendR5G6B5(uint16_t ColorA, uint16_t ColorB, uint8_t Alpha);

	static uint8_t CombineValues(uint8_t A, uint8_t B);

public:
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A;
};

static constexpr Color ColorBlack = { 0, 0, 0, 255 };
static constexpr Color ColorWhite = { 255, 255, 255, 255 };

static constexpr Color ColorDarkGray = { 100, 100, 100, 255 };
static constexpr Color ColorGray = { 127, 127, 127, 255 };
static constexpr Color ColorLightGray = { 186, 186, 186, 255 };

static constexpr Color ColorDarkRed = { 100, 0, 0, 255 };
static constexpr Color ColorRed = { 186, 0, 0, 255 };
static constexpr Color ColorLightRed = { 255, 0, 0, 255 };

static constexpr Color ColorDarkGreen = { 0, 100, 0, 255 };
static constexpr Color ColorGreen = { 0, 186, 0, 255 };
static constexpr Color ColorLightGreen = { 0, 255, 0, 255 };

static constexpr Color ColorBlue = { 0, 0, 186, 255 };
static constexpr Color ColorLightBlue = { 0, 0, 255, 255 };

#endif