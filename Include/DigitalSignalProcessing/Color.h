#pragma once
#ifndef COLOR_H
#define COLOR_H

#include "DataTypes.h"

struct Color
{
public:
	Color(void);

	Color(uint8 R, uint8 G, uint8 B);

	Color(uint8 R, uint8 G, uint8 B, uint8 A);

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

	uint16 R5G6B5(void);

	static uint16 BlendR5G6B5(uint16 ColorA, uint16 ColorB, uint8 Alpha);

	static uint8 CombineValues(uint8 A, uint8 B);

public:
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;
};

static const Color ColorBlack = { 0, 0, 0, 255 };
static const Color ColorWhite = { 255, 255, 255, 255 };

static const Color ColorDarkGray = { 100, 100, 100, 255 };
static const Color ColorGray = { 127, 127, 127, 255 };
static const Color ColorLightGray = { 186, 186, 186, 255 };

static const Color ColorDarkRed = { 100, 0, 0, 255 };
static const Color ColorRed = { 186, 0, 0, 255 };
static const Color ColorLightRed = { 255, 0, 0, 255 };

static const Color ColorDarkGreen = { 0, 100, 0, 255 };
static const Color ColorGreen = { 0, 186, 0, 255 };
static const Color ColorLightGreen = { 0, 255, 0, 255 };

static const Color ColorBlue = { 0, 0, 186, 255 };
static const Color ColorLightBlue = { 0, 0, 255, 255 };

#endif