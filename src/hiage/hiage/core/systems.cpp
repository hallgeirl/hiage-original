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
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicalComponent, MovableComponent>();
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
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicalComponent, RenderableComponent>();

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
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicsComponent, MovableComponent>();

	for (auto& t : componentTuples)
	{
		auto& physical = std::get<0>(t);
		auto& movement = std::get<1>(t);
		movement->accellerate(magnitude * frameTime, Vector2<double>(0, -1));
	}
}


hiage::HumanControllerSystem::HumanControllerSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void hiage::HumanControllerSystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<HumanControllerComponent, MovableComponent>();
	for (auto& t : componentTuples)
	{
		auto& controller = std::get<0>(t);
		auto& movement = std::get<1>(t);

		// TODO: Implement proper key bindings and generally more flexibility here.
		// Might need a component that defines accelleration magnitude as well. Or perhaps this could go into the movement component?
		auto& inputManager = game.getInputManager();
		if (inputManager.keyDown(SDL_SCANCODE_RIGHT))
		{
			movement->accellerate(100. * frameTime, Vector2<double>(1, 0));
		}
		if (inputManager.keyDown(SDL_SCANCODE_LEFT))
		{
			movement->accellerate(100. * frameTime, Vector2<double>(-1, 0));
		}
		if (inputManager.keyDown(SDL_SCANCODE_DOWN))
		{
			movement->accellerate(100. * frameTime, Vector2<double>(0, -1));
		}
		if (inputManager.keyDown(SDL_SCANCODE_UP))
		{
			movement->accellerate(100. * frameTime, Vector2<double>(0, 1));
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
