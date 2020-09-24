/*!
	\file $(filename)
	\author $(author)

*/

#include <hiage/core/objectfactory.h>
#include <hiage/core/script_lua.h>
#include "mariostate.h"

using namespace hiage;
using namespace std;


MarioState::MarioState(hiage::Game &game) : MapState(game)
{

}


MarioState::~MarioState()
{

}

void MarioState::render(double frametime)
{
	gamemap.render();
}

void MarioState::update(double frametime)
{
    gamemap.update(frametime);
}

void MarioState::handleEvents(double frametime)
{

}

void MarioState::resume()
{

}

void MarioState::pause()
{

}

void MarioState::destroy()
{

}

void MarioState::init()
{
	//game->scriptVM.loadScript("TestScript", "data/scripts/testscript.lua");
	//game->scriptVM.runScript("TestScript");

	//gamemap.createFromFile("data/maps/testlevel1.map");
}


