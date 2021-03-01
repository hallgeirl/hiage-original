/*
File: entity.cpp

Author: Hallgeir Lien

Description: Implementation of PhysicalEntity

Copyright (C) Hallgeir Lien - 2008
*/

#include "../util/exceptions.h"
#pragma warning(push, 0)
#include <tinyxml.h>
#pragma warning(pop)
#include "entity.hpp"
#include "game.hpp"

using namespace hiage;
using namespace std;

int Entity::entityCounter = 1;

Entity::Entity(const std::string& name) : _name(name)
{
    _entityId = Entity::entityCounter++;

}

Entity::~Entity()
{
}

int hiage::Entity::getEntityId() const
{
    return _entityId;
}

const std::string& hiage::Entity::getName() const
{
    return _name;
}
