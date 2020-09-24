#pragma once

#include "typedefs.h"

#include <map>
#include <string>

//struct Mix_Chunk;
//struct Mix_Music;
typedef struct Mix_Chunk Mix_Chunk;
typedef struct _Mix_Music Mix_Music;

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

