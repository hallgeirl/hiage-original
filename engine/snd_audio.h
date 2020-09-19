#pragma once

#include <map>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "com_typedefs.h"


class __IMPORTEXPORT AudioManager
{
private:
	std::map<std::string, Mix_Chunk *> sounds;
	std::map<std::string, Mix_Music *> music;

public:
	bool initialize(int frequency, int bits);

	void loadWav(std::string name, std::string file);
	void loadOgg(std::string name, std::string file);

	void playWav(std::string name);
	void playOgg(std::string name, bool loop);

	void stopOgg();
};

