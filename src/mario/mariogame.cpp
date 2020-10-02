/*!
	\file $(filename)
	\author $(author)
*/

#include "mariogame.h"
#include "mariostate.h"

using namespace hiage;
using namespace std;

MarioGame::~MarioGame()
{
}

void MarioGame::onInit()
{
	display.setZoom(400);
	//load all resources
	try
	{
		loadResources("data/textures", ResourceTypeEnum::TEXTURE);
		loadResources("data/sprites", ResourceTypeEnum::SPRITE);
		loadResources("data/tilesets", ResourceTypeEnum::TILESET);
		loadResources("data/objects", ResourceTypeEnum::OBJECT);
		loadResources("data/fonts", ResourceTypeEnum::FONT);
		loadResources("data/objects_json", ResourceTypeEnum::OBJECT_JSON);

		audio.loadWav("NormalHit1", "data/audio/hit_normal1.wav");
		audio.loadWav("NormalHit2", "data/audio/hit_normal2.wav");
		audio.loadWav("NormalHit3", "data/audio/hit_normal3.wav");
		audio.loadWav("NormalHit4", "data/audio/hit_normal4.wav");
		audio.loadWav("NormalHit5", "data/audio/hit_normal5.wav");
		audio.loadWav("NormalHit6", "data/audio/hit_normal6.wav");
		audio.loadWav("NormalHit7", "data/audio/hit_normal7.wav");
		audio.loadWav("HardHit", "data/audio/hit_hard.wav");

		audio.loadWav("NormalJump", "data/audio/jump_normal.wav");
		audio.loadWav("Shrink", "data/audio/shrink.wav");
		audio.loadWav("Kick", "data/audio/kick.wav");
		audio.loadWav("MarioDie", "data/audio/mario_die.wav");
		audio.loadWav("MushroomSpawn", "data/audio/mushroom_spawn.wav");
		audio.loadWav("Grow", "data/audio/grow.wav");
		audio.loadWav("Coin", "data/audio/coin.wav");
		audio.loadWav("ExtraLife", "data/audio/extralife.wav");
		audio.loadWav("Thomp", "data/audio/thomp.wav");

		audio.loadOgg("Theme", "data/audio/music/menutheme.ogg");
		audio.loadOgg("Overworld1", "data/audio/music/overworld1.ogg");
		audio.loadOgg("Finish", "data/audio/music/finish.ogg");

		MarioState * newstate = new MarioState(*this);
		setGameState(newstate);
	}
	catch (Exception &e)
	{
		cout << "Error loading resources. Aborting! " << e.what() << endl;
		stop();
		return;
	}
}
