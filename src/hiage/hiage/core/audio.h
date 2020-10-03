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
	std::string dataRoot;

public:
	AudioManager(const std::string& dataRoot);
	bool initialize(int frequency, int bits);

	void loadWav(const std::string& name, const std::string& file);
	void loadOgg(const std::string& name, const std::string& file);

	void playWav(const std::string& name);
	void playOgg(const std::string& name, bool loop);

	void stopOgg();
};

