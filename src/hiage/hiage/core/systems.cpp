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


hiage::ControllerSystem::ControllerSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void hiage::ControllerSystem::update(double)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<ControllerComponent, ControllerStateComponent>();
	for (auto& t : componentTuples)
	{
		auto& controllerData = std::get<1>(t)->getData();
		auto& controllerState = std::get<2>(t);

		if (controllerData.controllerType == "human")
		{
			auto& inputManager = game.getInputManager();
			auto actions = inputManager.getControllerActions();
			controllerState->setData(actions);
		}
		else if (controllerData.controllerType == "constant")
		{
			std::unordered_set<std::string> actions;
			for (auto& a : controllerData.constantActions)
				actions.insert(a);

			controllerState->setData(actions);
		}
	}
}

void hiage::ObjectObjectCollisionDetectionSystem::update(double frameTime)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<CollidableComponent, PositionComponent, VelocityComponent>();

	// Sort by x coordinate
	gameState.getEntityManager().sortEntitiesByPosition();

	BoundingPolygon polygon1, polygon2;
	// Check for collisions
	for (int i = 0; i < componentTuples.size(); i++)
	{
		auto& c1 = componentTuples[i];
		auto entityId1 = get<0>(c1);
		const auto& pos1 = get<2>(c1)->getData();
		const auto& vel1 = get<3>(c1)->getData();
		polygon1 = get<1>(c1)->getData();

		polygon1.translate(pos1);

		for (int j = i + 1; j < componentTuples.size(); j++)
		{
			auto& c2 = componentTuples[j];
			auto entityId2 = get<0>(c2);
			const auto& pos2 = get<2>(c2)->getData();
			auto relativeFrameVelocity = (vel1 - get<3>(c2)->getData()) * frameTime;
			polygon2 = get<1>(c2)->getData();

			polygon2.translate(pos2);

			// Distance check: If object 2 is further to the right than it's possible to move in one frame, we can skip this check.
			// For subsequent objects, they will be even more to the right, so we can break out early here.
			auto xDistance = polygon2.getLeft() - polygon1.getRight();
			if (xDistance > relativeFrameVelocity.getX())
				break;

			auto result = collisionTester.testCollision(polygon1, polygon2, relativeFrameVelocity);

			if (result.willIntersect || result.isIntersecting)
			{
				// Add two collision events - object 1 colliding with object 2, and the other way around.
				gameState.getEventQueue().enqueue(std::make_unique<ObjectObjectCollisionEvent>(ObjectObjectCollisionData{
					.entityId1 = entityId1,
					.entityId2 = entityId2,
					.collisionResult = result
					}));

				gameState.getEventQueue().enqueue(std::make_unique<ObjectObjectCollisionEvent>(ObjectObjectCollisionData{
					.entityId1 = entityId2,
					.entityId2 = entityId1,
					.collisionResult = result
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

		// Get bounding polygons for tiles within the sprite's overlap
		vector<BoundingPolygon> tilePolygons = tilemap.getBoundingPolygonsInRect(objectPolygon.getLeft() + vel.getX() * frameTime, objectPolygon.getTop() + vel.getY() * frameTime, objectPolygon.getRight() + vel.getX() * frameTime, objectPolygon.getBottom() + vel.getY() * frameTime);
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

void hiage::BlockingTileSystem::update(double frameTime)
{
	auto& events = gameState.getEventQueue().peekAll(BuiltinEventTypes::Collision_ObjectTile);

	for (auto& evt : events)
	{
		auto& myEvt = dynamic_cast<ObjectTileCollisionEvent&>(*evt);

		auto components = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent>(myEvt.getData().entityId);

		auto& pos = std::get<0>(components)->getData();
		auto& vel = std::get<1>(components)->getData();
		
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
