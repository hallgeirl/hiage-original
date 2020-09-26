/*!
	\file entitymanager.hpp
	\author Hallgeir Lien

	This file contains the declaration and implementation of the object factory classes that will help create objects.
*/
#pragma once

#include "game.h"
#include "entity.h"
#include "../util/exceptions.h"
#include "../gfx/fonts.h"

#include <string>
#include <map>

namespace hiage
{
	/*!
		Factory for game entities. This is used to create the in-game objects.
	*/
	//class Game;

	class EntityManager
	{
	protected:
		typedef PhysicalEntity *(*CreateObjectFunc)(std::string objectName, Game * game);

		std::map<std::string, CreateObjectFunc> objects;

	public:
		EntityManager();


		EntityManager(EntityManager &obj);

		~EntityManager();

		/*template <typename T>
		T queryComponentGroup()
		{
			// TODO - Use archetypes here later

		}*/

		/*!

		*/
		PhysicalEntity * createObject(std::string objectName, Game * game, const GameState& gameState);

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
