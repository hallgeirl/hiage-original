/*
File: entity.hpp

Author: Hallgeir Lien

Description: Header for the most basic entity classes/interfaces:
- EntityBase (Abstract)
- RenderableEntity (Abstract)
- MovableEntity (Abstract)
- PhysicalEntity
*/

#pragma once

#include "../gfx/sprite.h"
#include "typedefs.h"
#include "../util/vector2.h"
#include "../gfx/tilemap.hpp"

#include "components.hpp"
#include <iostream>
#include <memory>
#include <string>

namespace hiage
{
	class Game;
	class GameState;
	
	//base entity interface
	class Entity
	{
	private:
		int _entityId;
		static int entityCounter;
		std::string _name; // The name of the object descriptor used to create this entity. This is needed in order to propely save maps.
	public:
		Entity(const std::string& name);
		Entity(const Entity&) = delete;
		~Entity();
		int getEntityId() const;
		const std::string& getName() const;
	};
}
