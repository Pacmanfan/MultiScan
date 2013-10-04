#include "Color.h"

Color::Color(void)
{
	R = 128;
	G = 128;
	B = 128;
}

Color::~Color(void)
{
}

void Color::operator = (Color p)
{
	R = p.R;
	G = p.G;
	B = p.B;
}