/*!
	\file com_objectfactory.h
	\author Hallgeir Lien

	This file contains the declaration and implementation of the object factory classes that will help create objects.
*/
#pragma once

#include "objectfactory.h"
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

	class EntityFactory
	{
	protected:
		typedef PhysicalEntity *(*CreateObjectFunc)(std::string objectName, Game * game);

		std::map<std::string, CreateObjectFunc> objects;

	public:
		EntityFactory();


		EntityFactory(EntityFactory &obj);

		~EntityFactory();


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
