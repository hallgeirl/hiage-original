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
#include "entity.h"
#include "game.h"

using namespace hiage;
using namespace std;

int Entity::entityCounter = 1;

Entity::Entity()
{
    entityId = Entity::entityCounter++;
}

Entity::~Entity()
{
}

int hiage::Entity::getEntityId() const
{
    return entityId;
}
