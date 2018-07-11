#include "stdafx.h"
#include "MakeText.h"



Text MakeText::setParam(Font font, int characterSize, Color FillColor, int position[2])
{
	Text textName;

	textName.setFont(font);
	textName.setCharacterSize(characterSize);
	textName.setFillColor(FillColor);
	textName.setPosition(position[0], position[1]);

	return textName;
}

Text MakeText::setParam(Font font, int characterSize, Color FillColor, int position[2], String textString)
{
	Text textName;

	textName.setFont(font);
	textName.setCharacterSize(characterSize);
	textName.setFillColor(FillColor);
	textName.setPosition(position[0], position[1]);
	textName.setString(textString);

	return textName;
}
