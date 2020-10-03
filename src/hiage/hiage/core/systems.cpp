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
		auto& physical = std::get<1>(t);
		auto& movement = std::get<2>(t);
		
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
		auto& phyiscal = std::get<1>(t);
		auto& renderable = std::get<2>(t);

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
		auto& physical = std::get<1>(t);
		auto& movement = std::get<2>(t);

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
		auto& controller = std::get<1>(t);
		auto& movement = std::get<2>(t);

		// TODO: Implement proper key bindings and generally more flexibility here.
		// Might need a component that defines accelleration magnitude as well. Or perhaps this could go into the movement component?
		auto& vel = movement->getData();

		auto& inputManager = game.getInputManager();
		double magnitude = 100. * frameTime;

		if (inputManager.keyDown("goRight"))
		{
			vel.add(Vector2<double>(1, 0) * magnitude);
		}
		if (inputManager.keyDown("goLeft"))
		{
			vel.add(Vector2<double>(-1, 0) * magnitude);
		}
		if (inputManager.keyDown("crouch"))
		{
			vel.add(Vector2<double>(0, -1) * magnitude);
		}
		if (inputManager.keyDown("lookUp"))
		{
			vel.add(Vector2<double>(0, 1) * magnitude);
		}
	}
}

// Used for sorting based on the x coordinate of objects
template<typename ...TAll>
struct PositionComponentComparator {
	bool operator()(std::tuple<int, std::shared_ptr<TAll>...> a,
		std::tuple<int, std::shared_ptr<TAll>...> b) const
	{
		return get<1>(a)->getData().getX() < get<1>(b)->getData().getX();
	}
};

void hiage::ObjectObjectCollisionDetectionSystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent, BoundingBoxComponent>();
	PositionComponentComparator<PositionComponent, VelocityComponent, CollidableComponent, BoundingBoxComponent> comp;

	// Sort by x coordinate
	std::sort(componentTuples.begin(), componentTuples.end(), comp);
	
	// Check for collisions
	for (int i = 0; i < componentTuples.size(); i++)
	{
		auto& c1 = componentTuples[i];
		auto entityId1 = get<0>(c1);
		const auto& pos1 = get<1>(c1)->getData();
		const auto& vel1 = get<2>(c1)->getData();

		for (int j = i + 1; j < componentTuples.size(); j++)
		{
			auto& c2 = componentTuples[j];
			auto entityId2 = get<0>(c2);
			const auto& pos2 = get<1>(c2)->getData();
			const auto& vel2 = get<2>(c2)->getData();
			
			
			// Position of both objects at the time of collision
			Vector2<double> colPos1, colPos2;

			//check the distance between them
			//TODO: Put in a better distance check based on relative velocities.
			double distance = (get<1>(c1)->getData() - get<1>(c2)->getData()).length();
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
				auto colRect1 = get<4>(c1)->getData();
				auto colRect2 = get<4>(c2)->getData();

				//get the collision rect for both objects
				colRect1.translate(pos1);

				colRect2.translate(pos2);

				//do the rects intersect?
				if (colRect1.getLeft() < colRect2.getRight() && colRect1.getRight() > colRect2.getLeft() && colRect1.getTop() > colRect2.getBottom() && colRect1.getBottom() < colRect2.getTop())
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
				gameState.getEventQueue().enqueue(std::make_unique<ObjectObjectCollisionEvent>(ObjectObjectCollisionData{
									.entityId1 = entityId1,
									.entityId2 = entityId2,
									.objectPosition1 = currentPosition1,
									.objectPosition2 = currentPosition2
									/*, .normalVector = Vector2<double>(0,1) todo - implement collision detection that actually finds the normal vector for the collision (see my C# hiage)*/
					}));
			}
		}
	}
}


SystemsFactory::SystemsFactory(Game& game, GameState& gameState) : game(game), gameState(gameState)
{
}

void hiage::ObjectTileCollisionDetectionSystem::update(double frameTime)
{
	if (!tilemap.isLoaded())
		return;

	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent, BoundingBoxComponent>();

	for (auto& c : componentTuples)
	{
		const auto entityId = get<0>(c);
		const auto& pos = get<1>(c)->getData();
		const auto& vel = get<2>(c)->getData();

		Vector2<double> dvelocity = vel * frameTime;
		Vector2<double> currentPosition = pos;

		bool collided = false;

		//get the collision box of the object
		BoundingPolygon objectPolygon = get<4>(c)->getData();
		objectPolygon.translate(currentPosition);

		int tileSize = tilemap.getTileSize();

		// Get bounding box for tiles within the sprite's overlap
		BoundingBox<double> tilerect = tilemap.getTilesInRect(objectPolygon.getLeft() - tileSize, objectPolygon.getTop() + tileSize, objectPolygon.getRight() + tileSize, objectPolygon.getBottom() - tileSize);

		if (((tilerect.right - tilerect.left) > 0) && ((tilerect.top - tilerect.bottom) > 0))
		{
			for (int x = (int)tilerect.left; x <= (int)tilerect.right; x++)
			{
				for (int y = (int)tilerect.bottom; y <= (int)tilerect.top; y++)
				{
					// todo: should do a single object->multiple polygon test
					vector<BoundingPolygon> tilePolygons;
					int blockType = tilemap.getTileset()->getTile(tilemap.getTile(x, y, 0)).block;
					// block == 1 means fully blocking - i.e. all edges are blocking.
					if (blockType == 1)
					{
						BoundingPolygon p;
						
						// lower left
						p.addVertex((double)x * tileSize, (double)y * tileSize);
						// upper left
						p.addVertex((double)x * tileSize, (double)y * tileSize +  tileSize);
						// upper right
						p.addVertex((double)x * tileSize + tileSize, (double)y * tileSize + tileSize);
						// lower right
						p.addVertex((double)x * tileSize + tileSize, (double)y * tileSize);
						p.buildNormals();
						tilePolygons.push_back(p);
					}
					// block == 2 means blocking from above, but not from below
					else if (blockType == 2)
					{
						BoundingPolygon p;

						// upper left
 						p.addVertex((double)x * tileSize, (double)y * tileSize + tileSize);
						// upper right
						p.addVertex((double)x * tileSize + tileSize, (double)y * tileSize + tileSize);
						p.buildNormals();
						tilePolygons.push_back(p);
					}

					auto result = collisionTester.testCollision(objectPolygon, vel * frameTime, tilePolygons, -1);
					if (result.hasIntersected || result.isIntersecting)
					{
						gameState.getEventQueue().enqueue(std::make_unique<ObjectTileCollisionEvent>(ObjectTileCollisionData{
							.entityId = entityId,
							.objectPosition = currentPosition,
							.tilePosition = Vector2<int>(x,y),
							.normalVector = result.hitNormal
							}));
					}
				}
			}
		}
	}
}

void hiage::BlockingTileSystem::update(double frameTime)
{
	unique_ptr<Event> evt;
	while ((evt = gameState.getEventQueue().dequeue(BuiltinEventTypes::Collision_ObjectTile)) != nullptr)
	{
		auto& myEvt = dynamic_cast<ObjectTileCollisionEvent&>(*evt);

		auto& components = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, BoundingBoxComponent>(myEvt.getData().entityId);
		auto& pos = std::get<0>(components)->getData();
		auto& vel = std::get<1>(components)->getData();
		auto& bb = std::get<2>(components)->getData();
		
		// TODO - Make this a proper collision handling, respecting the normal vector of the collision.
		vel.setY(0);
		pos.setY(myEvt.getData().tilePosition.getY() * 16. + 16 - bb.getBottom());
	}
}

void hiage::AnimationSystem::update(double frameTime)
{
	auto& componentTuples = gameState.getEntityManager().queryComponentGroup<RenderableComponent, StateComponent>();

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
		auto& renderable = std::get<1>(t);
		auto& stateName = std::get<2>(t)->getData();
		
		auto& sprite = renderable->getData();
		auto& animName = sprite.getCurrentAnimationName();
		
		if (stateName != animName)
		{
			sprite.playAnimation(stateName);
		}
	}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           