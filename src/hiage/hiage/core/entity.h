/*
File: entity.h

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
#include "../gfx/tilemap.h"

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
		int entityId;
		static int entityCounter;
	public:
		Entity();
		//Entity(const Entity&) = delete;
		~Entity();
		int getEntityId() const;
	};
}
