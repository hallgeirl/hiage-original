                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     #include "systems.hpp"
#include "game.hpp"
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
	{
		auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent>();
		for (auto& t : componentTuples)
		{
			auto& physical = std::get<1>(t);
			auto& movement = std::get<2>(t);
		
			auto vel = movement->getData();
			auto pos = physical->getData();
			physical->getData().set(pos + vel * frametime);
		}
	}

	auto componentTuples = gameState.getEntityManager().queryComponentGroup<VelocityComponent, SpeedLimitComponent>();
	for (auto& t : componentTuples)
	{
		auto& movement = std::get<1>(t)->getData();
		auto& limit = std::get<2>(t)->getData();

		auto limitX = limit.speedLimit.getX();
		auto limitY = limit.speedLimit.getY();
		if (limitX >= 0 && abs(movement.getX()) > limitX)
		{
			if (movement.getX() > 0)
				movement.setX(limitX);
			else
				movement.setX(-limitX);
		}
		if (limitY >= 0 && abs(movement.getY()) > limitY)
		{
			if (movement.getY() > 0)
				movement.setY(limitY);
			else
				movement.setY(-limitY);
		}
	}
}

ObjectRenderingSystem::ObjectRenderingSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void ObjectRenderingSystem::update(double frameTime)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, RenderableComponent>();

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
		auto entityId = std::get<0>(t);
		auto& phyiscal = std::get<1>(t);
		auto& renderable = std::get<2>(t);

		auto& pos = phyiscal->getData();
		auto& sprite = renderable->getData();
		
		//check if the object is inside the viewport
		if ((pos.getX() + sprite.getWidth() >= viewLeft) && (pos.getX() <= viewRight))
		{
			if ((pos.getY() + sprite.getHeight() >= viewBottom) && (pos.getY() <= viewTop))
			{
				auto vel = gameState.getEntityManager().queryComponentGroup<VelocityComponent>(entityId);
				double velocity = 0;
				if (vel != nullptr)
					velocity = vel->getData().getX();

				auto state = gameState.getEntityManager().queryComponentGroup<StateComponent>(entityId);
				
				bool hflip = false, vflip = false;
				if (state != nullptr)
				{
					auto& metadata = state->getData().metadata;
					if (metadata.contains("x-flip") && get<int>(metadata.at("x-flip")) != 0)
						hflip = true;
					if (metadata.contains("y-flip") && get<int>(metadata.at("y-flip")) != 0)
						vflip = true;
				}

				// Render sprite
				sprite.render(renderer, pos, ObjectZ::MIDDLE, 0.f, hflip, vflip);

				// Update animations
				sprite.updateAnimation(frameTime, velocity);
			}
		}
	}
}

hiage::PhysicsSystem::PhysicsSystem(Game& game, GameState& gameState, double gravity) : System(game, gameState), _gravity(gravity)
{
}

void hiage::PhysicsSystem::update(double frameTime)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicsComponent, VelocityComponent>();

	for (auto& t : componentTuples)
	{
		auto& physics = std::get<1>(t);
		auto& movement = std::get<2>(t);
		auto& vel = movement->getData();

		auto& physicsProps = physics->getData();
		if (physicsProps.hasGravity)
			vel.add(Vector2<double>(0, -1) * _gravity * frameTime);
		
		if (physicsProps.airResistance > 0)
			vel.subtract(vel.normalized() * physicsProps.airResistance);
	}
}


hiage::HumanControllerSystem::HumanControllerSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void hiage::HumanControllerSystem::update(double frameTime)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<HumanControllerComponent, ControllerStateComponent>();
	for (auto& t : componentTuples)
	{
		auto& controllerState = std::get<2>(t);

		auto& inputManager = game.getInputManager();
		auto actions = inputManager.getControllerActions();
		controllerState->setData(actions);
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
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent>();
	PositionComponentComparator<PositionComponent, VelocityComponent, CollidableComponent> comp;

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
			for (int k = 0; k < dspeed; k++)
			{
				auto colRect1 = get<3>(c1)->getData();
				auto colRect2 = get<3>(c2)->getData();

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

static int frameCounter = 0;
void hiage::ObjectTileCollisionDetectionSystem::update(double frameTime)
{
	frameCounter++;
	if (!tilemap.isLoaded())
		return;

	auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent>();

	for (auto& c : componentTuples)
	{
		const auto entityId = get<0>(c);
		const auto& pos = get<1>(c)->getData();
		const auto& vel = get<2>(c)->getData();

		Vector2<double> dvelocity = vel * frameTime;
		Vector2<double> currentPosition = pos;

		//get the collision box of the object
		BoundingPolygon objectPolygon = get<3>(c)->getData();
		objectPolygon.translate(currentPosition);

		int tileSize = tilemap.getTileSize();

		// Get bounding box for tiles within the sprite's overlap
		BoundingBox<double> tilerect = tilemap.getTilesInRect(objectPolygon.getLeft() - tileSize, objectPolygon.getTop() + tileSize, objectPolygon.getRight() + tileSize, objectPolygon.getBottom() - tileSize);

		if (((tilerect.right - tilerect.left) > 0) && ((tilerect.top - tilerect.bottom) > 0))
		{
			vector<BoundingPolygon> tilePolygons;
			for (int x = (int)tilerect.left; x <= (int)tilerect.right; x++)
			{
				for (int y = (int)tilerect.bottom; y <= (int)tilerect.top; y++)
				{
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

				}
			}
			
			for (int axis = 0; axis <= 1; axis++)
			{
				auto result = collisionTester.testCollision(objectPolygon, vel * frameTime, tilePolygons, axis);
				if (result.willIntersect || result.isIntersecting)
				{
					gameState.getEventQueue().enqueue(std::make_unique<ObjectTileCollisionEvent>(ObjectTileCollisionData{
						.entityId = entityId,
						.collisionResult = result
						}));
				}
			}
		}
	}
}

void hiage::BlockingTileSystem::update(double frameTime)
{
	auto& events = gameState.getEventQueue().peekAll(BuiltinEventTypes::Collision_ObjectTile);

	for (auto& evt : events)
	{
		auto& myEvt = dynamic_cast<ObjectTileCollisionEvent&>(*evt);

		auto components = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent>(myEvt.getData().entityId);

		auto& pos = std::get<0>(components)->getData();
		auto& vel = std::get<1>(components)->getData();
		auto& bb = std::get<2>(components)->getData();
		
		auto& collisionResult = myEvt.getData().collisionResult;

		// collisionTimeFactor is the fraction of the velocity that should be applied to move the object to the position of the collision
		auto collisionTimeFactor = frameTime * collisionResult.collisionTime;
		// Calculate the per-axis velocity
		Vec2d deltaPos(vel.getX() * collisionTimeFactor * collisionResult.axis.getX(), vel.getY() * collisionTimeFactor * collisionResult.axis.getY());
		pos.add(deltaPos - vel * 1.0e-6);

		// Adjust the velocity according to the hit normal
		vel.set(vel - (collisionResult.hitNormal * (1.0 + 0) * vel.dot(myEvt.getData().collisionResult.hitNormal)));
	}
}

void hiage::AnimationSystem::update(double)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<RenderableComponent, StateComponent>();

	for (auto& t : componentTuples)
	{
		auto& renderable = std::get<1>(t);
		auto& state = std::get<2>(t)->getData();
		auto& sprite = renderable->getData();

		sprite.playAnimation(state.stateName, false);
	}
}

void hiage::ObjectTrackingSystem::update(double frameTime)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, TrackableComponent>();

	if (componentTuples.size() == 0)
		return;

	auto trackingTarget = componentTuples[0];
	auto& trackingPos = get<1>(trackingTarget)->getData();
	auto& trackingVel = get<2>(trackingTarget)->getData();

	auto trackingComponentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, TrackingComponent>();
	for (auto& c : trackingComponentTuples)
	{
		auto& tracking = get<3>(c)->getData();
		string mode = "elastic";
		if (tracking.mode.length() > 0)
			mode = tracking.mode;
		
		auto& pos = get<1>(c)->getData();
		auto& vel = get<2>(c)->getData();
		if (mode == "fixed")
		{
			pos.set(trackingPos);
		} 
		else if (mode == "elastic")
		{
			auto diff = (trackingPos + trackingVel) - pos;

			vel.scale(0.5);
			auto accelVector = diff * 100. + trackingVel;
			vel.add(accelVector * frameTime);
		}
	}
}

void hiage::CameraSystem::update(double)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, CameraComponent>();

	for (auto& c : componentTuples)
	{
		auto& pos = get<1>(c)->getData();

		// TODO - Make boundaries configurable.
		auto zoom = game.getDisplay().getZoom();
		auto aspect = game.getDisplay().getAspectRatio();
		auto leftBoundary = zoom * aspect;
		auto bottomBoundary = zoom;


		game.getDisplay().setCamPosition(std::max(pos.getX(), leftBoundary), std::max(pos.getY(), bottomBoundary));
	}
}
