/*!
	\file entitymanager.hpp
	\author Hallgeir Lien

	This file contains the declaration and implementation of the object factory classes that will help create objects.
*/
#pragma once

#include "components.hpp"
#include "entity.hpp"
#include "resourcedescriptors.hpp"

#include "../util/exceptions.h"
#include "../gfx/fonts.hpp"

#include <string>
#include <map>
#include <memory>
#include <flecs.h>

namespace hiage
{
	/*!
		Factory for game entities. This is used to create the in-game objects.
	*/
	class Game;
	class GameState;

	class EntityManager
	{
	private:
		flecs::world& _ecs;
		std::vector<flecs::entity> _entities;
		const Game& _game;
		const GameState& _gameState;

		int _cacheVersion;

	public:
		EntityManager(const Game& game, const GameState& gameState, flecs::world& ecs);


		EntityManager(EntityManager&) = delete; // Doesn't make sense to copy

		~EntityManager();

		/*!
		Create entity
		*/
		flecs::entity& createEntity(std::string objectName, const std::unordered_map<std::string, ComponentProperties>& componentRuntimeProperties);

		void addComponentToEntity(flecs::entity& entity, const std::string& componentType, ComponentProperties& componentProperties);
		void removeComponentFromEntity(int entityId, const std::string& componentType);

		void destroyAll();
	};

	class FontFactory
	{
    public:
	    FontFactory();

	    ~FontFactory();

	    Font * createFont(const std::string& fontName, Game * game);
	};
}
