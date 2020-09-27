#include "systems.hpp"
#include "game.h"

using namespace std;
using namespace hiage;


System::System(Game& game, ComponentManager& componentManager, EntityManager& entityManager) : componentManager(componentManager), entityManager(entityManager), game(game)
{
}

System::~System()
{
}

MovementSystem::MovementSystem(Game& game, ComponentManager& componentManager, EntityManager& entityManager) : System(game, componentManager, entityManager)
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

ObjectRenderingSystem::ObjectRenderingSystem(Game& game, ComponentManager& componentManager, EntityManager& entityManager, Renderer& renderer) : System(game, componentManager, entityManager), renderer(renderer)
{
}

void ObjectRenderingSystem::update(double frameTime)
{
	auto& componentTuples = this->entityManager.queryComponentGroup<PhysicalComponent, RenderableComponent>(PhysicalComponent::TYPEID, RenderableComponent::TYPEID);

	Display& disp = game.getDisplay();
	Renderer& renderer = disp.getRenderer();
	double aspect = disp.getAspectRatio();
	double zoom = disp.getZoom();
	double camX = disp.getCamX();
	double camY = disp.getCamY();

	double viewLeft = camX - (zoom * aspect);
	double viewRight = camX + (zoom * aspect);
	double viewTop = camY + zoom;
	double viewBottom = camY - zoom;

	for (auto& t : componentTuples)
	{
		auto& phyiscal = std::get<0>(t);
		auto& renderable = std::get<1>(t);

		auto& pos = phyiscal->getPosition();
		auto& sprite = renderable->getSprite();
		
		//check if the object is inside the viewport
		if ((pos.getX() + sprite.getWidth() >= viewLeft) && (pos.getX() <= viewRight))
		{
			if ((pos.getY() + sprite.getHeight() >= viewBottom) && (pos.getY() <= viewTop))
			{
				// Render sprite
				sprite.render(renderer, pos, ObjectZ::MIDDLE);
			}
		}
	}
}


SystemsFactory::SystemsFactory(ComponentManager& componentManager, EntityManager& entityManager, Game& game) : componentManager(componentManager), entityManager(entityManager), game(game)
{

}

unique_ptr<System> hiage::SystemsFactory::createSystem(std::string name)
{
	if (name == "movement")
		return make_unique<MovementSystem>(game, componentManager, entityManager);

	if (name == "objectrendering")
		return make_unique<ObjectRenderingSystem>(game, componentManager, entityManager, game.getDisplay().getRenderer());
	
	throw runtime_error("Unknown system name: " + name);
}


