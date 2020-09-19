/*!
	\file com_objectfactory.h
	\author Hallgeir Lien

	This file contains the declaration and implementation of the object factory classes that will help create objects.
*/
#pragma once

#include <string>
#include <map>
#include "com_game.h"
#include "com_exceptions.h"
#include "game_entity.h"
#include "util_fonts.h"

namespace Hinage
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
		PhysicalEntity * createObject(std::string objectName, Game * game);

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
