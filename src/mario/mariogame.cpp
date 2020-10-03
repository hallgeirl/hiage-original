/*!
	\file $(filename)
	\author $(author)
*/

#include "mariogame.hpp"
#include "mariostate.hpp"

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
		loadResources("textures", ResourceTypeEnum::TEXTURE);
		loadResources("sprites", ResourceTypeEnum::SPRITE);
		loadResources("tilesets", ResourceTypeEnum::TILESET);
		loadResources("objects", ResourceTypeEnum::OBJECT);
		loadResources("fonts", ResourceTypeEnum::FONT);

		audio.loadWav("NormalHit1", "audio/hit_normal1.wav");
		audio.loadWav("NormalHit2", "audio/hit_normal2.wav");
		audio.loadWav("NormalHit3", "audio/hit_normal3.wav");
		audio.loadWav("NormalHit4", "audio/hit_normal4.wav");
		audio.loadWav("NormalHit5", "audio/hit_normal5.wav");
		audio.loadWav("NormalHit6", "audio/hit_normal6.wav");
		audio.loadWav("NormalHit7", "audio/hit_normal7.wav");
		audio.loadWav("HardHit", "audio/hit_hard.wav");

		audio.loadWav("NormalJump", "audio/jump_normal.wav");
		audio.loadWav("Shrink", "audio/shrink.wav");
		audio.loadWav("Kick", "audio/kick.wav");
		audio.loadWav("MarioDie", "audio/mario_die.wav");
		audio.loadWav("MushroomSpawn", "audio/mushroom_spawn.wav");
		audio.loadWav("Grow", "audio/grow.wav");
		audio.loadWav("Coin", "audio/coin.wav");
		audio.loadWav("ExtraLife", "audio/extralife.wav");
		audio.loadWav("Thomp", "audio/thomp.wav");

		audio.loadOgg("Theme", "audio/music/menutheme.ogg");
		audio.loadOgg("Overworld1", "audio/music/overworld1.ogg");
		audio.loadOgg("Finish", "audio/music/finish.ogg");

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
