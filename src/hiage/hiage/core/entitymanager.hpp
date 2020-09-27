/*!
	\file entitymanager.hpp
	\author Hallgeir Lien

	This file contains the declaration and implementation of the object factory classes that will help create objects.
*/
#pragma once

#include "entity.h"
#include "../util/exceptions.h"
#include "../gfx/fonts.h"

#include <string>
#include <map>
#include <memory>

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
		std::vector<std::unique_ptr<Entity>> entities;
		std::map<int, std::vector<std::shared_ptr<Component>>> components; // Entity ID -> component list
		const Game& game;
		const GameState& gameState;

	public:
		EntityManager(const Game& game, const GameState& gameState);


		EntityManager(EntityManager&) = delete; // Doesn't make sense to copy

		~EntityManager();

		
		int getObjectCount();

		// TODO - remove this eventually
		/*
		Entity& getObjectByIndex(int index)
		{
			return *entities[index];
		}

		
		template <typename T>
		T& getObjectByIndex(int index)
		{
			return (T&)(*entities[index]);
		}

		void removeObjectByIndex(int index)
		{
			entities.erase(entities.begin() + index);
		}*/


		template <typename T>
		std::vector<std::shared_ptr<T>> queryComponentGroup(int typeId)
		{
			// TODO - Use archetypes here later - for now, just use this naive implementation

			std::vector<std::shared_ptr<T>> results;
			for (auto& e : entities)
			{
				auto& componentList = components[e->getEntityId()]
				for (auto& c : componentList)
				{ 
					if (c->getTypeId() == typeId)
						results.push_back(std::shared_ptr<T>(std::dynamic_pointer_cast<T>(c)));
				}
			}

			return results;
		}

		template <typename T1, typename T2>
		std::vector<std::tuple<std::shared_ptr<T1>, std::shared_ptr<T2>>> queryComponentGroup(int typeId1, int typeId2)
		{
			// TODO - Use archetypes here later - for now, just use this naive implementation
			// This is quite ugly - but will have to do for now

			std::vector<std::tuple<std::shared_ptr<T1>, std::shared_ptr<T2>>> results;
			for (auto& e : entities)
			{
				bool found1 = false, found2 = false;
				std::shared_ptr<T1> c1; 
				std::shared_ptr<T2> c2;
				auto& componentList = components[e->getEntityId()];
				for (auto& c : componentList)
				{
					if (c->getTypeId() == typeId1)
					{
						c1 = std::dynamic_pointer_cast<T1>(c);
						found1 = true;
					}
					if (c->getTypeId() == typeId2)
					{
						c2 = std::dynamic_pointer_cast<T2>(c);
						found2 = true;
					}
				}
				if (found1 && found2)
				{
					results.push_back(std::make_tuple(c1, c2));
				}
			}

			return results;
		}

		/*!

		*/
		Entity& createEntity(std::string objectName, const std::map<std::string, void*>& attributes);

		/*template <typename ObjectClass>
		void registerObject(std::string objectName)
		{
			objects[objectName] = &createEntity<ObjectClass>;
		}*/
	};

	class FontFactory
	{
    public:
	    FontFactory();

	    ~FontFactory();

	    Font * createFont(std::string fontName, Game * game);
	};
}