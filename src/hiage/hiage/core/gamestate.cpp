#include "gamestate.hpp"
#include "game.hpp"

using namespace hiage;

/*
	Gamestate class
*/

GameState::GameState(Game& game) : _game(game), _systemsManager(_ecs), _entityManager(game, *this, _ecs), _componentManager(game)
{

}

GameState::~GameState()
{
}

SystemsManager& hiage::GameState::getSystemsFactory()
{
	return _systemsManager;
}


void GameState::update(double frametime)
{
	_ecs.progress((float)frametime);
}

void hiage::GameState::cleanupFrame()
{
	auto remainingEvents = _eventQueue.dequeueAll();
	_eventQueue.clear();

	// Uncomment for debugging
	/*
	for (auto& evt : remainingEvents)
	{
		std::clog << "Warning: Unhandled event of type " << evt->getType() << " at end of update cycle." << std::endl;
	}
	*/
}

void GameState::changeState(Game* game, GameState* state)
{
	std::clog << "Game state: Changed state.\n" << std::flush;
	game->setGameState(state);
}

const ComponentManager& hiage::GameState::getComponentManager() const
{
	return _componentManager;
}

EntityManager& hiage::GameState::getEntityManager()
{
	return _entityManager;
}

EventQueue& hiage::GameState::getEventQueue()
{
	return _eventQueue;
}
