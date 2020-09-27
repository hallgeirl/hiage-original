#include "systems.hpp"
#include "game.h"

using namespace std;
using namespace hiage;


System::System(Game& game, GameState& gameState) : game(game), gameState(gameState)
{
}

System::~System()
{
}

MovementSystem::MovementSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void MovementSystem::update(double frametime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicalComponent, MovableComponent>(PhysicalComponent::TYPEID, MovableComponent::TYPEID);
	for (auto& t : componentTuples)
	{
		auto& physical = std::get<0>(t);
		auto& movement = std::get<1>(t);
		
		auto vel = movement->getVelocity();
		auto pos = physical->getPosition();
		physical->setPosition(pos + vel * frametime);
	}
}

ObjectRenderingSystem::ObjectRenderingSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void ObjectRenderingSystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicalComponent, RenderableComponent>(PhysicalComponent::TYPEID, RenderableComponent::TYPEID);

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

hiage::GravitySystem::GravitySystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void hiage::GravitySystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicsComponent, MovableComponent>(PhysicsComponent::TYPEID, MovableComponent::TYPEID);

	for (auto& t : componentTuples)
	{
		auto& physical = std::get<0>(t);
		auto& movement = std::get<1>(t);
		movement->accellerate(magnitude * frameTime, Vector2<double>(0, -1));
	}
}


SystemsFactory::SystemsFactory(Game& game, GameState& gameState) : game(game), gameState(gameState)
{
}

unique_ptr<System> hiage::SystemsFactory::createSystem(std::string name)
{
	if (name == "movement")
		return make_unique<MovementSystem>(game, gameState);

	if (name == "objectrendering")
		return make_unique<ObjectRenderingSystem>(game, gameState);

	if (name == "gravity")
		return make_unique<GravitySystem>(game, gameState);

	throw runtime_error("Unknown system name: " + name);
}

