#pragma once
#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include <SFML/Audio.hpp>
#include <map>

using namespace sf;
using namespace std;

class SoundEffects
{
public:
	SoundEffects();
	static Sound& GetSound(std::string const& filename);

private:
	// A map container from th STL
	// that holds related pairs of String and Sounds
	std::map<std::string, Sound>m_Sound;

	// A pointer of the same type as the class itself
	// the one and only instance
	static SoundEffects* m_s_Instance;
};

#endif // !SOUNDEFFECTS_H