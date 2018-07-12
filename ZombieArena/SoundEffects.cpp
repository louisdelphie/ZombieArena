#include "stdafx.h"
#include "SoundEffects.h"

// Include the "assert feature"
#include <assert.h>

SoundEffects* SoundEffects::m_s_Instance = nullptr;

SoundEffects::SoundEffects()
{
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

sf::Sound& SoundEffects::GetSound(std::string const& filename)
{
	auto& m = m_s_Instance->m_Sound;

	// Create an iterator to hold a key-value-pair (kvp)
	// and search for the required kvp
	// using the passed in filename
	auto keyValuePair = m.find(filename);
	// auto is equivelant of map<string, Sound>::iterator

	// Did we find a match?
	if (keyValuePair != m.end())
	{
		// Yes
		// Return the sound,
		// the second part of the kvp, the texture
		return keyValuePair->second;
	}
	else
	{
		SoundBuffer soundBuffer;
		soundBuffer.loadFromFile(filename);
		// Filename not found
		// Create a new key value pair using the filename
		auto& sound = m[filename];
		// Load the texture from file in the usual way
		sound.setBuffer(soundBuffer);

		// Return the texture to the calling code
		return sound;
	}
}
	
