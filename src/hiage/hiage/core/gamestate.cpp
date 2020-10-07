#include "gamestate.hpp"
#include "game.hpp"

using namespace hiage;

/*
	Gamestate class
*/

GameState::GameState(Game& game) : gameInstance(game), systemsFactory(game, *this), entityManager(game, *this), componentManager(game)
{

}

GameState::~GameState()
{
}

const SystemsFactory& hiage::GameState::getSystemsFactory() const
{
	return systemsFactory;
}


void GameState::update(double frametime)
{
	for (auto const& sys : systems)
	{
		sys->update(frametime);
	}
	auto remainingEvents = eventQueue.dequeueAll();
	eventQueue.clear();

	for (auto& evt : remainingEvents)
	{
		// Uncomment for debugging
		// std::clog << "Warning: Unhandled event of type " << evt->getType() << " at end of update cycle." << std::endl;
	}
}

void GameState::changeState(Game* game, GameState* state)
{
	std::clog << "Game state: Changed state.\n" << std::flush;
	game->setGameState(state);
}

const ComponentManager& hiage::GameState::getComponentManager() const
{
	return componentManager;
}

EntityManager& hiage::GameState::getEntityManager()
{
	return entityManager;
}

EventQueue& hiage::GameState::getEventQueue()
{
	return eventQueue;
}
