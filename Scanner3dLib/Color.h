#pragma once

class Color
{
public:
	unsigned char R,G,B;
	Color(void);
	~Color(void);
	void Color::operator = (Color p);
};
