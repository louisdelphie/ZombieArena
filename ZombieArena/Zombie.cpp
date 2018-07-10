#include "stdafx.h"
#include "Zombie.h"
#include "TextureHolder.h"
#include <cstdlib>
#include <ctime>

using namespace std;

void Zombie::spawn(float startX, float startY, ZombieType type, int seed)
{
	switch (type)
	{
	case ZombieType::Bloater:
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/bloater.png"));

		m_Speed = 40;
		m_Health = 5;
		break;
	case ZombieType::Chaser:
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/chaser.png"));

		m_Speed = 70;
		m_Health = 1;
		break;
	case ZombieType::Crawler:
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/crawler.png"));

		m_Speed = 20;
		m_Health = 3;
		break;
	}

	// Modify the speed to make the zombie unique
	// Every zombie is unique. Create a speed modifier
	srand((int)time(0) * seed);

	// Somewhere between 80 and 100
	float modifier = (rand() % MAX_VARRIANCE) + OFFSET;

	// Express this as a fraction of 1
	modifier /= 100; // Now equals between .7 and 1
	m_Speed *= modifier;

	// Initialize its location
	m_Position.x = startX;
	m_Position.y = startY;

	// Set its origin to its center
	m_Sprite.setOrigin(25, 25);

	// Set its position
	m_Sprite.setPosition(m_Position);
}

bool Zombie::hit()
{
	m_Health--;

	if (m_Health < 0)
	{
		//dead 
		m_Alive = false;
		m_Sprite.setTexture(TextureHolder::GetTexture("graphics/blood.png"));

		return true;
	}

	// injured but not dead yet
	return false;
}

bool Zombie::isAlive()
{
	return m_Alive;
}

FloatRect Zombie::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

Sprite Zombie::getSprite()
{
	return m_Sprite;
}

void Zombie::update(float elapsedtime, Vector2f playerLocation)
{
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	// Update the zombie position variables
	if (playerX > m_Position.x)
	{
		m_Position.x = m_Position.x + m_Speed * elapsedtime;
	}

	if (playerY > m_Position.y)
	{
		m_Position.y = m_Position.y + m_Speed * elapsedtime;
	}

	if (playerX < m_Position.x)
	{
		m_Position.x = m_Position.x - m_Speed * elapsedtime;
	}

	if (playerY < m_Position.y)
	{
		m_Position.y = m_Position.y - m_Speed * elapsedtime;
	}

	// Move the sprite 
	m_Sprite.setPosition(m_Position);

	// Face the sprite in the correct direction
	float angle = (atan2(playerY - m_Position.y, playerX - m_Position.x) * 180) / 3.141;

	m_Sprite.setRotation(angle);
}