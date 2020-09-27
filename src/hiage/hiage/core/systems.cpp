#include "systems.hpp"
#include "game.h"
#include <SDL/SDL.h>

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
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent>();
	for (auto& t : componentTuples)
	{
		auto& physical = std::get<0>(t);
		auto& movement = std::get<1>(t);
		
		auto vel = movement->getData();
		auto pos = physical->getData();
		physical->getData().set(pos + vel * frametime);
	}
}

ObjectRenderingSystem::ObjectRenderingSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void ObjectRenderingSystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, RenderableComponent>();

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

		auto& pos = phyiscal->getData();
		auto& sprite = renderable->getData();
		
		//check if the object is inside the viewport
		if ((pos.getX() + sprite.getWidth() >= viewLeft) && (pos.getX() <= viewRight))
		{
			if ((pos.getY() + sprite.getHeight() >= viewBottom) && (pos.getY() <= viewTop))
			{
				// Render sprite
				sprite.render(renderer, pos, ObjectZ::MIDDLE);
				sprite.updateAnimation(frameTime);
			}
		}
	}
}

hiage::GravitySystem::GravitySystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void hiage::GravitySystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicsComponent, VelocityComponent>();

	for (auto& t : componentTuples)
	{
		auto& physical = std::get<0>(t);
		auto& movement = std::get<1>(t);

		auto& vel = movement->getData();
		vel.add(Vector2<double>(0, -1) * magnitude * frameTime);
	}
}


hiage::HumanControllerSystem::HumanControllerSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void hiage::HumanControllerSystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<HumanControllerComponent, VelocityComponent>();
	for (auto& t : componentTuples)
	{
		auto& controller = std::get<0>(t);
		auto& movement = std::get<1>(t);

		// TODO: Implement proper key bindings and generally more flexibility here.
		// Might need a component that defines accelleration magnitude as well. Or perhaps this could go into the movement component?
		auto& vel = movement->getData();

		auto& inputManager = game.getInputManager();
		double magnitude = 100. * frameTime;

		if (inputManager.keyDown(SDL_SCANCODE_RIGHT))
		{
			vel.add(Vector2<double>(1, 0) * magnitude);
		}
		if (inputManager.keyDown(SDL_SCANCODE_LEFT))
		{
			vel.add(Vector2<double>(-1, 0) * magnitude);
		}
		if (inputManager.keyDown(SDL_SCANCODE_DOWN))
		{
			vel.add(Vector2<double>(0, -1) * magnitude);
		}
		if (inputManager.keyDown(SDL_SCANCODE_UP))
		{
			vel.add(Vector2<double>(0, 1) * magnitude);
		}
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

	if (name == "humancontroller")
		return make_unique<HumanControllerSystem>(game, gameState);

	throw runtime_error("Unknown system name: " + name);
}
