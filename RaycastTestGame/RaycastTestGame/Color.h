#pragma once

#include <string>

typedef unsigned char byte;

using std::string;

struct ColorA;

struct Color
{
public:
	Color();
	Color(unsigned int color);
	Color(byte r, byte g, byte b);

public:
	string ToStringValue(byte& colorValue);
	ColorA RGBToRGBA();
	string ToANSIEscape();

	friend Color& operator/=(Color& color, float divisor);

public:
	byte r;
	byte g;
	byte b;

};

struct ColorA
{
public:
	ColorA();
	ColorA(unsigned int color);
	ColorA(byte r, byte g, byte b, byte a);

public:
	string ToStringValue(byte& colorValue);
	Color RGBAToRGB();

	Color LayerRGBAOnRGB(Color layerColor) const;


	friend ColorA& operator/=(ColorA& color, float divisor);

public:
	byte r;
	byte g;
	byte b;
	byte a;
};
