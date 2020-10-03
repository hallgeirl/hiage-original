#pragma once

#include "typedefs.h"
#include "entitymanager.hpp"
#include "systems.hpp"
#include "components.hpp"
#include "events.hpp"

namespace hiage
{
	/*!
		Class for handling game states. This is an abstract class, so you need to derive it.
		The idea is that you can have a number of game states in a game.
		Each state can have its own code for handling input, updating whatever objects you got, rendering and so forth.
		For instance, you can derive a state called MapState which handles everything that happens on the map
		(like rendering the map, updating all objects on the map, handling the input and so forth), and you can have let's say a
		MainMenuState which renders the main menu etc.
	*/
	class __IMPORTEXPORT GameState
	{
	protected:
		ComponentManager	componentManager;
		SystemsFactory		systemsFactory;
		EntityManager		entityManager;
		EventQueue			eventQueue;

		Game& gameInstance;
		std::vector<std::unique_ptr<System>> systems;

	public:
		GameState(Game& game);
		GameState(const GameState&) = delete;
		virtual ~GameState();
		virtual void init() = 0;
		virtual void initScript() {}
		virtual void destroy() = 0;

		virtual void pause() {}
		virtual void resume() {}

		virtual void handleEvents(double frametime) = 0;
		virtual void update(double frametime);
		virtual void render() = 0;

		void changeState(Game* game, GameState* state);

		const ComponentManager& getComponentManager() const;
		const SystemsFactory& getSystemsFactory() const;

		EntityManager& getEntityManager();
		EventQueue& getEventQueue();
	};
}