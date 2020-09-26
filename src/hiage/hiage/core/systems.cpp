#include "systems.hpp"
#include "game.h"

using namespace std;
using namespace hiage;


System::System(ComponentManager& componentManager) : componentManager(componentManager)
{
}

System::~System()
{
}

MovementSystem::MovementSystem(ComponentManager& componentManager) : System(componentManager)
{
}

void MovementSystem::update(double frametime)
{
	this->componentManager.getComponentsOfType("physical");
	//for (Component& c : )
}

ObjectRenderingSystem::ObjectRenderingSystem(ComponentManager& componentManager) : System(componentManager)
{
}

void ObjectRenderingSystem::update(double frameTime)
{
}


SystemsFactory::SystemsFactory(ComponentManager& componentManager) : componentManager(componentManager)
{

}

unique_ptr<System> hiage::SystemsFactory::createSystem(std::string name)
{
	if (name == "movement")
		return make_unique<MovementSystem>(componentManager);

	if (name == "objectrendering")
		return make_unique<ObjectRenderingSystem>(componentManager);
	
	throw runtime_error("Unknown system name: " + name);
}


