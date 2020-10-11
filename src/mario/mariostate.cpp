/*!
	\file $(filename)
	\author $(author)

*/

#include "mariostate.hpp"
#include "systems.hpp"

#include <hiage/core/entitymanager.hpp>
#include <hiage/core/script_lua.h>
#include "components.hpp"

using namespace hiage;
using namespace std;


MarioState::MarioState(hiage::Game &game) : MapState(game)
{
    auto sysFactory = getSystemsFactory();

    // Movement and controllers -- updates velocity
    systems.push_back(sysFactory.createSystem<HumanControllerSystem>());
    systems.push_back(sysFactory.createSystem<CharacterControllerSystem>());
    systems.push_back(sysFactory.createSystem<PhysicsSystem>(1000));

    // Collision detection -- will we collide in this frame?
    systems.push_back(sysFactory.createSystem<ObjectObjectCollisionDetectionSystem>());
    auto& tilemap = gamemap.getTilemap();
    systems.push_back(sysFactory.createSystem<ObjectTileCollisionDetectionSystem, const Tilemap&>(tilemap));

    // Collision handling
    systems.push_back(sysFactory.createSystem<BlockingTileSystem>());
    systems.push_back(sysFactory.createSystem<MarioCollisionResponseSystem>());

    // Movement
    systems.push_back(sysFactory.createSystem<MovementSystem>());

    systems.push_back(sysFactory.createSystem<CharacterStateMachineSystem>());

    // Rendering
    systems.push_back(sysFactory.createSystem<CameraSystem>());
    systems.push_back(sysFactory.createSystem<ObjectTrackingSystem>());
    systems.push_back(sysFactory.createSystem<AnimationSystem>());
    systems.push_back(sysFactory.createSystem<ObjectRenderingSystem>());


    /*
    Component factories
    */
    componentManager.addComponentFactory<GenericComponentFactory<BlockingComponent>>("blocking");
}


MarioState::~MarioState()
{

}

void MarioState::render()
{
	gamemap.render();
}

void MarioState::update(double frametime) 
{
	GameState::update(frametime);
    gamemap.update(frametime);
}

void MarioState::handleEvents(double)
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

	//gamemap.createFromFile("data/maps/testlevel1.json");
}


