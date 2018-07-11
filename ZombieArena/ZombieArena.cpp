// ZombieArena.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sstream>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"

using namespace sf;


int main()
{
	TextureHolder holder;
	// The game will always be in one of four states
	enum class State
	{
		PAUSED, LEVELING_UP, GAME_OVER, PLAYING
	};

	// Start with the GAME_OVER state
	State state = State::GAME_OVER;

	// Get the screen resolution and create as SFML window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

	// Create an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Here is our clock for timing everything
	Clock clock;
	// How long has the PLAYING state been active
	Time gameTimeTotal;

	// Where is the mouse in relation to world coordinates
	Vector2f mouseWorldPosition;
	// Where is the mouse in relation to screen coordinates
	Vector2i mouseScreenPosition;

	// Create an instance of the Player class
	Player player;

	// The boundaries of the arena
	IntRect arena;

	// Create the background
	VertexArray background;
	// Load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");


	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;

	// 100 bullets should do
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;

	// When was the fire button last pressed?
	Time lastPressed;

	// Hide the mouse pointer and replace it with crossshair 
	window.setMouseCursorVisible(true);
	Sprite	spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");

	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	// Create a couple of pickups
	Pickup healthPickup(PickupType::Health);
	Pickup ammoPickup(PickupType::Ammo);

	// About the game
	int score = 0;
	int hiScore = 0;

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background1.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);

	// Load the font
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");

	// Paused
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \n to continue");

	//Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");

	// LEVELING UP
	int levelUpPosition[] = { 150,250 };
	std::stringstream levelUpstream;
	levelUpstream <<
		"1- Increased rate of fire" <<
		"\n2- Increased clip size (next reload)" <<
		"\n3- Increased max health" <<
		"\n4- Increased run speed" <<
		"\n5- More and better health pickups" <<
		"\n6- More and better ammo pickups";
	Text levelUpText = MakeText::setParam(font, 80, Color::White,levelUpPosition ,levelUpstream.str());

	// Ammo
	int ammoTextPosition[] = { 200,980 };
	Text ammotext = MakeText::setParam(font, 55, Color::White, ammoTextPosition);

	// Score
	int scoreTextPosition[] = { 20,0 };
	Text scoreText = MakeText::setParam(font, 55, Color::White, scoreTextPosition);

	// Hi Score
	int hiScoreTextPosition[] = { 1400, 0 };
	std::stringstream s;
	s << "Hi Score:" << hiScore;
	Text hiScoreText = MakeText::setParam(font, 55, Color::White, hiScoreTextPosition, s.str());

	// Zombies remaining
	int zombiesRemainingTextPosition[] = { 1500, 980 };
	Text zombiesRemainingText = MakeText::setParam(font, 55, Color::White, zombiesRemainingTextPosition, "Zombies: 100");

	// Wave number
	int wave = 0;
	int waveNumberTextPosition[] = { 1250,980 };
	Text waveNumberText = MakeText::setParam(font, 55, Color::White, waveNumberTextPosition, "Wave: 0");

	// Health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);
	
	// When did we last update the HUD?
	int framesSinceLastHUDUpdate = 0;

	// How often (in frame) should we update the HUD
	int fpsMeasurementFrameInterval = 1000;

	//The main game loop
	while (window.isOpen())
	{
		/*
		************
		Handle input
		************
		*/

		// Handle events
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed)
			{
				// Pause a game while playing
				if (event.key.code == Keyboard::Return && state == State::PLAYING)
				{
					state = State::PAUSED;
				}
				else if (event.key.code == Keyboard::Return && state == State::PAUSED) // Restart while paused
				{
					state = State::PLAYING;
					// Reset the clock so there isn't a frame jump
					clock.restart();
				}
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER) // Start a new game while in GAME_OVER state
				{
					state = State::LEVELING_UP;
				}

				if (state == State::PLAYING)
				{
					// Reloading 
					if (event.key.code == Keyboard::R)
					{
						if (bulletsSpare >= clipSize)
						{
							// Plenty of bullets. Reload.
							bulletsInClip = clipSize;
							bulletsSpare -= clipSize;
						}
						else if (bulletsSpare > 0)
						{
							// Only few bullets left 
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
						}
						else
						{
							
						}
					}
				}

			}

		} //End event polling

		// Handle the player quitting 
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Handle controls while playing -- Handle the pressing and releasing of the WASD keys
		if (state == State::PLAYING)
		{
			// Handle the pressing and releasing of the WASD keys
			if (Keyboard::isKeyPressed(Keyboard::W))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::S))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::A))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}

			if (Keyboard::isKeyPressed(Keyboard::D))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

			// Fire a bullet
			if (Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (can_I_Fire(gameTimeTotal, lastPressed, fireRate, bulletsInClip))
				{
					// Pass the center of the player
					// and the center of the crosshair
					// to the shoot function
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);

					currentBullet++;
					if (currentBullet > 99)
					{
						currentBullet = 0;
					}
					lastPressed = gameTimeTotal;
					bulletsInClip--;
				}

			} // End fire a bullet

		}// End WASD while playing

		// Handle the levelling up state
		if (state == State::LEVELING_UP)
		{
			// Handle the player levelling up 
			if (event.key.code == Keyboard::Num1)
			{
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num2)
			{
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num3)
			{
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num4)
			{
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num5)
			{
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num6)
			{
				state = State::PLAYING;
			}

			if (state == State::PLAYING)
			{
				// Prepare the level
				// We will modify the next two lines later
				arena.width = 500;
				arena.height = 500;
				arena.left = 0;
				arena.top = 0;
				
				// Pass the vertex array by reference
				// to the createBackground function
				int tileSize = createBackground(background,arena);
				

				// Spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				// Configure the pickups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);

				// Create a horde of zombies
				numZombies = 10;

				// Delete the previously allocated memory (if it exists)
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;

				// Reset the clock so there isn't a frame jump
				clock.restart();
			}
		}// End levelling up

		/*
		****************
		UPDATE THE FRAME
		****************
		*/
		if (state == State::PLAYING)
		{
			// Update the delta time
			Time dt = clock.restart();

			// Update the total game time
			gameTimeTotal += dt;

			// Make a decimal fraction of 1 from the delta time
			float dtAsSeconds = dt.asSeconds();

			// Where is the mouse pointer
			mouseScreenPosition = Mouse::getPosition();

			// Convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			// Set the crosshair to the mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);

			// Update the player 
			player.update(dtAsSeconds, Mouse::getPosition());

			// Make a note of the player new position
			Vector2f playerPosition(player.getCenter());

			// Make the view centre around the player
			mainView.setCenter(player.getCenter());

			// Loop through each Zombie and update them
			for (int i = 0; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
				{
					zombies[i].update(dt.asSeconds(), playerPosition);
				}
			}

			// Update any bullets that are in-flight
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					bullets[i].update(dtAsSeconds);
				}
			}

			// Update the pickups 
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			// Collision detection
			// Have any zombies been shot?
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < numZombies; j++)
				{
					if (bullets[i].isInFlight() && zombies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
						{
							// Stop the bullet
							bullets[i].stop();

							// Register the hit and see if it was a kill
							if (zombies[j].hit())
							{
								// not just a hit but a kill too
								score += 10;
								if (score >= hiScore)
								{
									hiScore = score;
								}
								numZombiesAlive--;
								// when all the zombies are dead (again)
								if (numZombiesAlive == 0)
								{
									state = State::LEVELING_UP;
								}
							}
						}
					}
				}
			}// End zombie being shot

			// Have any zombies touched the player
			for (int i = 0; i < numZombies; i++)
			{
				if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive())
				{
					if (player.hit(gameTimeTotal))
					{
						// More here later
					}

					if (player.getHealth() <= 0)
					{
						state = State::GAME_OVER;
					}
				}
			}// End player touched

			// Has the player touched health pickup
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.increaseHealthLevel(healthPickup.gotIt());
			}

			// Has the player touched ammo pickup
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				bulletsSpare += ammoPickup.gotIt();
			}

			// size up the health bar
			healthBar.setSize(Vector2f(player.getHealth() * 3, 50));

			// Incrementthe number of frames since the previous update
			framesSinceLastHUDUpdate++;

			// re-calculare every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{
				//update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombiesAlive;

				// Update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammotext.setString(ssAmmo.str());

				// Update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the zombies remaining text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());

				framesSinceLastHUDUpdate = 0;

			} // End HUD update

		}// End updating the scene

		/*
		**************
		Draw the scene
		**************
		*/

		if (state == State::PLAYING)
		{
			window.clear();

			// set th emainView to be displayed in the window
			// And draw everything related to it
			window.setView(mainView);
			
			// Draw the background
			window.draw(background, &textureBackground);

			// Draw the zombies
			for (int i = 0; i < numZombies; i++)
			{
				window.draw(zombies[i].getSprite());
			}

			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}

			// Draw the player
			window.draw(player.getSprite());

			// Draw the pickups, if currently spawned
			if (ammoPickup.isSpawned())
			{
				window.draw(ammoPickup.getSprite());
			}

			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
			}

			// Draw the crosshair
			window.draw(spriteCrosshair);
		}

		if (state == State::LEVELING_UP)
		{

		}

		if (state == State::PAUSED)
		{

		}

		if (state == State::GAME_OVER)
		{

		}

		window.display();
	} // End of main game loop

	// Delete the previously allocated memory (if it exists)
	delete[] zombies;
    return 0;

}

bool can_I_Fire(sf::Time &gameTimeTotal, sf::Time &lastPressed, float fireRate, int bulletsInClip)
{
	return gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0;
}



