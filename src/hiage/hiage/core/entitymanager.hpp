/*!
	\file entitymanager.hpp
	\author Hallgeir Lien

	This file contains the declaration and implementation of the object factory classes that will help create objects.
*/
#pragma once

#include "entity.hpp"
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
		std::vector<std::unique_ptr<Entity>> _entities;
		std::map<int, std::vector<std::shared_ptr<Component>>> _components; // Entity ID -> component list
		const Game& _game;
		const GameState& _gameState;

		/*
			Component query helpers.
		*/
		// Recursion: Find the next matching component
		template <class T, class TNext, class...TRest>
		std::tuple<std::shared_ptr<T>, std::shared_ptr<TNext>, std::shared_ptr<TRest>...> queryComponentGroupCore(std::vector<std::shared_ptr<Component>>::iterator& it, std::vector<std::shared_ptr<Component>>& componentList)
		{
			for (; it != componentList.end(); it++)
			{
				std::shared_ptr<T> c1;
				int typeId = T::TYPEID;
				if ((*it)->getTypeId() == typeId)
				{
					c1 = std::dynamic_pointer_cast<T>(*it);


					// Reset the iterator, begin again in the recursive call. This is certainly not very efficient, but will do OK until I get a proper archetype system up.
					it = componentList.begin();
					auto recursiveResult = queryComponentGroupCore<TNext, TRest...>(it, componentList);
					if (it == componentList.end())
						return std::tuple<std::shared_ptr<T>, std::shared_ptr<TNext>, std::shared_ptr<TRest>...>();

					auto t = std::tuple_cat(std::make_tuple(c1), recursiveResult);

					return t;
				}
			}
			return std::tuple<std::shared_ptr<T>, std::shared_ptr<TNext>, std::shared_ptr<TRest>...>();
		}

		// Base case: Returns a tuple with one element
		template <class T>
		std::tuple<std::shared_ptr<T>> queryComponentGroupCore(std::vector<std::shared_ptr<Component>>::iterator& it, std::vector<std::shared_ptr<Component>>& componentList)
		{
			for (; it != componentList.end(); it++)
			{
				std::shared_ptr<T> c1;


				if ((*it)->getTypeId() == T::TYPEID)
				{
					c1 = std::dynamic_pointer_cast<T>(*it);

					return std::make_tuple(c1);
				}
			}
			return std::tuple<std::shared_ptr<T>>(nullptr);
		}



	public:
		EntityManager(const Game& game, const GameState& gameState);


		EntityManager(EntityManager&) = delete; // Doesn't make sense to copy

		~EntityManager();
		
		template <typename T>
		std::shared_ptr<T> queryComponentGroup(int entityId)
		{
			// TODO - Use archetypes here later - for now, just use this naive implementation

			auto& componentList = _components[entityId];
			for (auto& c : componentList)
			{
				if (c->getTypeId() == T::TYPEID)
					return std::shared_ptr<T>(std::dynamic_pointer_cast<T>(c));
			}

			return nullptr;
		}
		
		template <typename T>
		std::vector<std::tuple<int, std::shared_ptr<T>>> queryComponentGroup()
		{
			// TODO - Use archetypes here later - for now, just use this naive implementation

			std::vector<std::tuple<int, std::shared_ptr<T>>> results;
			for (auto& e : _entities)
			{
				auto& res = queryComponentGroup<T>(e->getEntityId());
				if (res != nullptr)
					results.push_back(std::make_tuple(e->getEntityId(), res));
			}
			
			return results;
		}

		template <class T, class TNext, class...TRest>
		std::tuple<std::shared_ptr<T>, std::shared_ptr<TNext>, std::shared_ptr<TRest>...> queryComponentGroup(int entityId)
		{
			// TODO - Use archetypes here later - for now, just use this naive implementation
			// This is quite ugly - but will have to do for now

			auto& componentList = _components[entityId];
			auto it = componentList.begin();
			auto res = queryComponentGroupCore<T, TNext, TRest...>(it, componentList);
			if (it != componentList.end()) // If we haven't iterated to end(), we found one component of each type.
				return res;

			return std::tuple<std::shared_ptr<T>, std::shared_ptr<TNext>, std::shared_ptr<TRest>...>();
		}
		
		template <class T, class TNext, class...TRest>
		std::vector<std::tuple<int, std::shared_ptr<T>, std::shared_ptr<TNext>, std::shared_ptr<TRest>...>> queryComponentGroup()
		{
			// TODO - Use archetypes here later - for now, just use this naive implementation
			// This is quite ugly - but will have to do for now

			std::vector<std::tuple<int, std::shared_ptr<T>, std::shared_ptr<TNext>, std::shared_ptr<TRest>...>> results;
			for (auto& e : _entities)
			{
				auto res = queryComponentGroup<T, TNext, TRest...>(e->getEntityId());

				if (std::get<0>(res) != nullptr)
					results.push_back(std::tuple_cat(std::make_tuple(e->getEntityId()), res));
			}

			return results;
		}


		/*!
		Create entity
		*/
		void createEntity(std::string objectName, const ComponentProperties& runtimeProperties);
		void createEntity(std::string objectName, const std::unordered_map<std::string, ComponentProperties>& componentRuntimeProperties);
		const std::vector<std::unique_ptr<Entity>>& getEntities();
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
