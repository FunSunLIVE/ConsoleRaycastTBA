#include "Color.h"

#include <format>
#include <iostream>

Color::Color()
	: r{ 255 }, g{ 0 }, b{ 255 }
{
}

Color::Color(unsigned int color)
{
	char* byte = reinterpret_cast<char*>(&color);
	r = byte[0];
	g = byte[1];
	b = byte[2];
}

Color::Color(byte r, byte g, byte b)
	: r{ r }, g{ g }, b{ b }
{

}

string Color::ToStringValue(byte& colorValue)
{
	return string(std::to_string(short(colorValue)));
}

ColorA Color::RGBToRGBA(Color& color)
{
	return ColorA(r, g, b, 0);
}

string Color::ToANSIEscape()
{
	return std::format("\033[48;2;{};{};{}m ", r, g, b);
}

ColorA::ColorA()
	:r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 }
{
}

ColorA::ColorA(unsigned int color)
{
	char* byte = reinterpret_cast<char*>(&color);
	r = byte[0];
	g = byte[1];
	b = byte[2];
	a = byte[3];
}

ColorA::ColorA(byte r, byte g, byte b, byte a)
	:r{ r }, g{ g }, b{ b }, a{ a }
{
}

string ColorA::ToStringValue(byte& colorValue)
{
	return string(std::to_string(short(colorValue)));
}

Color ColorA::RGBAToRGB(ColorA& color)
{
	return Color(r, g, b);
}

Color& operator/=(Color& color, float divisor)
{
	if (divisor != 0.f) // Prevents dividing by 0
	{
		color.r /= divisor;
		color.g /= divisor;
		color.b /= divisor;
	}

	return color;
}
