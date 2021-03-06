#include "audio.h"
#include "../util/exceptions.h"

#include "../sdl-includes.h"
#include <iostream>
#include <filesystem>

using namespace std;
using namespace hiage;

AudioManager::AudioManager(const std::string& dataRoot) : dataRoot(dataRoot)
{
}

bool AudioManager::initialize(int frequency, int bits)
{
	clog << "Initializing AudioManager... Frequency " << frequency << ", bit depth " << bits << endl << flush;
	if (SDL_Init(SDL_INIT_AUDIO))
	{
		throw FatalException("ERROR: SDL Audio failed to initialize.");
	}

	if (frequency <= 0)
	{
		throw FatalException("ERROR: Invalid frequency selection.");
	}

	if ((bits != 8) && (bits != 16))
	{
		throw FatalException("ERROR: Invalid bit depth selection.");
	}

	Uint16 audioFormat = AUDIO_S8;

	switch (bits)
	{
	case 8:
		audioFormat = AUDIO_S8;
		break;

	case 16:
		audioFormat = AUDIO_S16SYS;
		break;
	}

	if (Mix_OpenAudio(frequency, audioFormat, 2, 1024))
	{
		throw FatalException("ERROR: Call to Mix_OpenAudio failed.");
	}

	clog << "OK: AudioManager initialized successfully.\n" << flush;

	return true;
}

void AudioManager::loadWav(const std::string& name, const std::string& file)
{
	std::filesystem::path root = dataRoot;
	auto fullPath = root / file;

	clog << "Loading WAV file " << fullPath.string() << ", and assigning name " << name << "...\n" << flush;
	Mix_Chunk * sound = Mix_LoadWAV(fullPath.string().c_str());

	if (!sound)
	{
		throw IOException("ERROR: WAV file was not loaded successfully.");
	}

	sounds[name] = sound;

	clog << "OK: WAV file was loaded successfully.\n" << flush;
}

void AudioManager::loadOgg(const std::string& name, const std::string& file)
{
	std::filesystem::path root = dataRoot;
	auto fullPath = root / file;

	clog << "Loading OGG file " << fullPath.string() << ", and assigning name " << name << "...\n" << flush;
	Mix_Music * mus = Mix_LoadMUS(fullPath.string().c_str());

	if (!mus)
	{
		throw IOException("ERROR: OGG file was not loaded successfully.");
	}

	music[name] = mus;

	clog << "OK: OGG file was loaded successfully.\n" << flush;
}

void AudioManager::playWav(const std::string& name)
{
	Mix_PlayChannel(-1, sounds[name], 0);
}

void AudioManager::playOgg(const std::string& name, bool loop)
{
    if (loop)
    {
        Mix_PlayMusic(music[name], -1);
    }
    else
    {
        Mix_PlayMusic(music[name], 0);
    }
}

void AudioManager::stopOgg()
{
    Mix_HaltMusic();
}
