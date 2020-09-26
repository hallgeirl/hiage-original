#include "systems.hpp"
#include "game.h"

using namespace std;
using namespace hiage;


System::System(ComponentManager& componentManager, EntityManager& entityManager) : componentManager(componentManager), entityManager(entityManager)
{
}

System::~System()
{
}

MovementSystem::MovementSystem(ComponentManager& componentManager, EntityManager& entityManager) : System(componentManager, entityManager)
{
}

void MovementSystem::update(double frametime)
{
	auto& componentTuples = this->entityManager.queryComponentGroup<PhysicalComponent, MovableComponent>(PhysicalComponent::TYPEID, MovableComponent::TYPEID);
	for (auto& t : componentTuples)
	{
		auto& physical = std::get<0>(t);
		auto& movement = std::get<1>(t);

		auto vel = movement->getVelocity();
		auto pos = physical->getPosition();
		physical->setPosition(pos + vel * frametime);
	}
}

ObjectRenderingSystem::ObjectRenderingSystem(ComponentManager& componentManager, EntityManager& entityManager, Renderer& renderer) : System(componentManager, entityManager), renderer(renderer)
{
}

void ObjectRenderingSystem::update(double frameTime)
{
	auto& componentTuples = this->entityManager.queryComponentGroup<PhysicalComponent, RenderableComponent>(PhysicalComponent::TYPEID, RenderableComponent::TYPEID);

	for (auto& t : componentTuples)
	{
		auto& phyiscal = std::get<0>(t);
		auto& renderable = std::get<1>(t);

		renderable->getSprite().render(renderer, ObjectZ::CLOSEST);
	}
}


SystemsFactory::SystemsFactory(ComponentManager& componentManager, EntityManager& entityManager, Game& game) : componentManager(componentManager), entityManager(entityManager), game(game)
{

}

unique_ptr<System> hiage::SystemsFactory::createSystem(std::string name)
{
	if (name == "movement")
		return make_unique<MovementSystem>(componentManager, entityManager);

	if (name == "objectrendering")
		return make_unique<ObjectRenderingSystem>(componentManager, entityManager, game.getDisplay().getRenderer());
	
	throw runtime_error("Unknown system name: " + name);
}


