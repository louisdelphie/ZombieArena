#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class MakeText
{
public:
	static Text setParam(Font font, int characterSize, Color FillColor, int position[2]);
	static Text setParam(Font font, int characterSize, Color FillColor, int position[2], String textString);

private:

};


