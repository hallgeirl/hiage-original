/*!
	\file $(filename)
	\author $(author)

*/

#include "mariostate.hpp"
#include "systems.hpp"

#include <hiage/core/entitymanager.hpp>
#include <hiage/core/script_lua.h>

using namespace hiage;
using namespace std;


MarioState::MarioState(hiage::Game &game) : MapState(game)
{
    auto sysFactory = getSystemsFactory();

    // Movement and controllers
    systems.push_back(sysFactory.createSystem<HumanControllerSystem>());
    systems.push_back(sysFactory.createSystem<MovementSystem>());
    systems.push_back(sysFactory.createSystem<GravitySystem>());

    // Collision detection
    systems.push_back(sysFactory.createSystem<ObjectObjectCollisionDetectionSystem>());
    auto& tilemap = gamemap.getTilemap();
    systems.push_back(sysFactory.createSystem<ObjectTileCollisionDetectionSystem, const Tilemap&>(tilemap));

    // Collision handling
    systems.push_back(sysFactory.createSystem<BlockingTileSystem>());

    systems.push_back(sysFactory.createSystem<CharacterStateMachineSystem>());

    // Rendering
    systems.push_back(sysFactory.createSystem<AnimationSystem>());
    systems.push_back(sysFactory.createSystem<ObjectRenderingSystem>());
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
	GameState::update(frametime);
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


