/*!
	\file $(filename)
	\author $(author)

*/

#include "mariostate.hpp"
#include "mariosystems.hpp"
#include "mariocomponents.hpp"

#include "events.hpp"

#include <hiage/core/entitymanager.hpp>
#include <hiage/core/script_lua.h>

using namespace hiage;
using namespace std;


MarioState::MarioState(hiage::Game &game) : MapState(game), _lives(5), _score(0), _coins(0), _fps(0)
{
    auto sysFactory = getSystemsFactory();

    // Movement and controllers -- updates velocity
    systems.push_back(sysFactory.createSystem<ControllerSystem>());
    systems.push_back(sysFactory.createSystem<CharacterControllerSystem>());
    systems.push_back(sysFactory.createSystem<PhysicsSystem>(800));

    // Collision detection -- will we collide in this frame?
    systems.push_back(sysFactory.createSystem<ObjectObjectCollisionDetectionSystem>());
    auto& tilemap = gamemap.getTilemap();
    systems.push_back(sysFactory.createSystem<ObjectTileCollisionDetectionSystem, const Tilemap&>(tilemap));

    // AI is dependent on collision detection - so we need to update AI after this.
    systems.push_back(sysFactory.createSystem<AISystem>());

    // Collision handling
    systems.push_back(sysFactory.createSystem<BlockingTileSystem>());
    systems.push_back(sysFactory.createSystem<MarioCollisionResponseSystem>());

    // Movement -- updates position
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
    _componentManager.addGenericComponentFactory<BlockingComponent>("blocking");
    _componentManager.addGenericComponentFactory<GroundMonsterControllerComponent>("groundmonstercontroller");

    _mainfont = game.createFont("SmallFont");
    game.getAudioManager().playOgg("Overworld1", true);

    gamemap.loadFromJson("maps/level1.json", false);
}


MarioState::~MarioState()
{

}

void MarioState::render()
{
	gamemap.render();

    _game.printTextFixed(_mainfont, string("Lives ") + _lives, 0, 0, ScreenHorizontalPosition::Left, ScreenVerticalPosition::Top, 0.3, -0.2);
    _game.printTextFixed(_mainfont, string("Coins ") + _coins, 0, -20, ScreenHorizontalPosition::Left, ScreenVerticalPosition::Top, 0.3, -0.2);
    _game.printTextFixed(_mainfont, string("Score ") + _score, -100, 0, ScreenHorizontalPosition::Right, ScreenVerticalPosition::Top, 0.3, -0.2);

    _game.printTextFixed(_mainfont, string("FPS ") + _fps, 0, 0, ScreenHorizontalPosition::Left, ScreenVerticalPosition::Bottom, 0.3, -0.2);
}

void MarioState::update(double frametime) 
{
	GameState::update(frametime);
    gamemap.update(frametime);

    auto& evts = _eventQueue.peekAll(MarioEventTypes::GainCoin);

    for (auto& evt : evts)
    {
        auto& myEvt = dynamic_cast<GainCoinEvent&>(*evt);
        _coins += myEvt.getData();
        if (_coins >= 100)
        {
            _coins = _coins % 100;
            _lives++;
            _game.getAudioManager().playWav("ExtraLife");
        }
        else
        {
            _game.getAudioManager().playWav("Coin");
        }
    }
    _fps = (int)((1. / frametime)*0.98 + _fps*0.02);
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


