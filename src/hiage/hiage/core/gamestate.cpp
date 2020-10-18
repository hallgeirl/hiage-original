#include "gamestate.hpp"
#include "game.hpp"

using namespace hiage;

/*
	Gamestate class
*/

GameState::GameState(Game& game) : _game(game), _systemsFactory(game, *this), _entityManager(game, *this), _componentManager(game)
{

}

GameState::~GameState()
{
}

const SystemsFactory& hiage::GameState::getSystemsFactory() const
{
	return _systemsFactory;
}


void GameState::update(double frametime)
{
	for (auto const& sys : systems)
	{
		sys->update(frametime);
	}
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
