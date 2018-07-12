#include "stdafx.h"
#include "helper.h"



namespace helper
{
	Text& MakeText::setParam(Font font, int characterSize, Color FillColor, int position[2])
	{
		Text textName;

		textName.setFont(font);
		textName.setCharacterSize(characterSize);
		textName.setFillColor(FillColor);
		textName.setPosition(position[0], position[1]);

		return textName;
	}

	Text& MakeText::setParam(Font font, int characterSize, Color FillColor, int position[2], String textString)
	{
		Text textName;

		textName.setFont(font);
		textName.setCharacterSize(characterSize);
		textName.setFillColor(FillColor);
		textName.setPosition(position[0], position[1]);
		textName.setString(textString);

		return textName;
	}

	bool Questions::can_I_Fire(sf::Time &gameTimeTotal, sf::Time &lastPressed, float fireRate, int bulletsInClip)
	{
		return gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0;
	}
}
