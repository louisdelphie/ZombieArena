#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

namespace helper
{
	class MakeText
	{
	public:
		static Text& setParam(Font font, int characterSize, Color FillColor, int position[2]);
		static Text& setParam(Font font, int characterSize, Color FillColor, int position[2], String textString);

	private:

	};

	class Questions
	{
	public:
		static bool can_I_Fire(sf::Time &gameTimeTotal, sf::Time &lastPressed, float fireRate, int bulletsInclip);
	};
}


