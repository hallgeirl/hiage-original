#include "systems.hpp"
#include "game.h"
#include <SDL/SDL.h>
#include <algorithm>

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

void hiage::ObjectObjectCollisionSystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent, BoundingBoxComponent>();
	PositionComponentComparator<PositionComponent, VelocityComponent, CollidableComponent, BoundingBoxComponent> comp;

	// Sort by x coordinate
	std::sort(componentTuples.begin(), componentTuples.end(), comp);
	
	// Check for collisions
	for (int i = 0; i < componentTuples.size(); i++)
	{
		auto& c1 = componentTuples[i];
		const auto& pos1 = get<0>(c1)->getData();
		const auto& vel1 = get<1>(c1)->getData();

		for (int j = i + 1; j < componentTuples.size(); j++)
		{
			auto& c2 = componentTuples[j];
			const auto& pos2 = get<0>(c2)->getData();
			const auto& vel2 = get<1>(c2)->getData();
			
			
			// Position of both objects at the time of collision
			Vector2<double> colPos1, colPos2;

			//check the distance between them
			//TODO: Put in a better distance check based on relative velocities.
			double distance = (get<0>(c1)->getData() - get<0>(c2)->getData()).length();
			if (distance > 50)
			{
				continue;
			}

			// Store the current position and speed for both objects
			double dspeed1 = vel1.length() * frameTime;
			double dspeed2 = vel2.length() * frameTime;

			//get the speed of the fastest object (the one that will move furthest during the next frame)
			int dspeed;
			if (dspeed1 > dspeed2)
			{
				dspeed = (int)ceil(dspeed1);
			}
			else
			{
				dspeed = (int)ceil(dspeed2);
			}

			//find the delta velocity vector (largest increments should have length 1)
			Vector2<double> dvelocity1 = vel1 * frameTime / dspeed;
			Vector2<double> dvelocity2 = vel2 * frameTime / dspeed;
		    //TODO: Optimization
			bool collided = false;
			auto currentPosition1 = Vector2<double>(pos1),
				 currentPosition2 = Vector2<double>(pos2);

			//check for collisions during the next frame
			for (int i = 0; i < dspeed; i++)
			{
				BoundingBox<double> colRect1 = get<3>(c1)->getData();
				BoundingBox<double> colRect2 = get<3>(c2)->getData();

				//get the collision rect for both objects
				colRect1.left += pos1.getX();
				colRect1.right += pos1.getX();
				colRect1.top += pos1.getY();
				colRect1.bottom += pos1.getY();

				colRect2.left += pos2.getX();
				colRect2.right += pos2.getX();
				colRect2.top += pos2.getY();
				colRect2.bottom += pos2.getY();

				//do the rects intersect?
				if (colRect1.left < colRect2.right && colRect1.right > colRect2.left && colRect1.top > colRect2.bottom && colRect1.bottom < colRect2.top)
				{
					collided = true;

					colPos1 = currentPosition1;
					colPos2 = currentPosition2;
					break;
				}

				//increment the position for a new check
				currentPosition1 -= dvelocity1;
				currentPosition2 -= dvelocity2;
			}

			if (collided)
			{
				// TODO: Handle collision!
			}
		}
	}
}


SystemsFactory::SystemsFactory(Game& game, GameState& gameState) : game(game), gameState(gameState)
{
}

void hiage::ObjectTileCollisionSystem::update(double frameTime)
{
	if (!tilemap.isLoaded())
		return; 

	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent, BoundingBoxComponent>();

	for (auto& c : componentTuples)
	{
		const auto& pos = get<0>(c)->getData();
		const auto& vel = get<1>(c)->getData();

		int dspeed = (int)ceil(vel.length() * frameTime);
		Vector2<double> dvelocity = vel * frameTime / dspeed;
		Vector2<double> currentPosition = pos;

		bool collided = false;

		for (int i = 0; i < dspeed; i++)
		{
			//get the collision box of the sprite
			BoundingBox<double> colRect = get<3>(c)->getData();
			colRect.left += currentPosition.getX();
			colRect.right += currentPosition.getX();
			colRect.bottom += currentPosition.getY();
			colRect.top += currentPosition.getY();

			int tileSize = tilemap.getTileSize();

			BoundingBox<double> tilerect = tilemap.getTilesInRect(colRect.left - tileSize, colRect.top + tileSize, colRect.right + tileSize, colRect.bottom - tileSize);
			if (((tilerect.right - tilerect.left) > 0) && ((tilerect.top - tilerect.bottom) > 0))
			{
				for (int x = (int)tilerect.left; x <= (int)tilerect.right; x++)
				{
					for (int y = (int)tilerect.bottom; y <= (int)tilerect.top; y++)
					{
						//onMapCollision(tilemap, x * tileSize, y * tileSize + tileSize, x * tileSize + tileSize, y * tileSize, tilemap->getTileset()->getTile(tilemap->getTile(x,y,0)).block, frametime);
						if (tilemap.getTileset()->getTile(tilemap.getTile(x, y, 0)).block > 0)
						{
							BoundingBox<double> tile;
							tile.left = (double)x * tileSize;
							tile.right = (double)x * tileSize + tileSize;
							tile.top = (double)y * tileSize + tileSize;
							tile.bottom = (double)y * tileSize;
							if (colRect.left < tile.right && colRect.right > tile.left && colRect.top > tile.bottom && colRect.bottom < tile.top)
							{
								// TODO - Handle collision
							}
						}
					}
				}
				break;
			}

			//increment the position by one step
			currentPosition -= dvelocity;
		}
	}
}
