#pragma once
#include "Zombie.h"

using namespace sf;

int createBackground(VertexArray& rVA, IntRect arena);
Zombie* createHorde(int numZombies, IntRect arena);

bool can_I_Fire(sf::Time &gameTimeTotal, sf::Time &lastPressed, float fireRate, int bulletsInclip);
