#include "systems.hpp"
#include "game.h"

using namespace hiage;


System::System(const Game& game) : game(game)
{
}

System::~System()
{
}


void ObjectRenderingSystem::invoke(double frameTime)
{
}

void MovementSystem::invoke(double frametime)
{

}
