#include "systems.hpp"
#include "game.hpp"
#include <SDL/SDL.h>
#include <algorithm>

using namespace std;
using namespace hiage;


// TODO flecs

System::System()
{
}

System::~System()
{
}

MovementSystem::MovementSystem() : System()
{
}

void MovementSystem::registerSystem(flecs::world& world)
{
	world.system<PositionComponent, VelocityComponent>()
		.each([](flecs::entity e, PositionComponent& position, VelocityComponent& velocity)
		{
			auto vel = velocity.getData();
			
			position.pos.set(position.pos + vel * e.delta_time());
		});
	

	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<VelocityComponent, SpeedLimitComponent>();
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
	}*/
}

ObjectRenderingSystem::ObjectRenderingSystem(Game& game) : System(), _game(game)
{
}

void ObjectRenderingSystem::registerSystem(flecs::world& world)
{
	world.system<PositionComponent, RenderableComponent, VelocityComponent*, StateComponent*>()
		.each([&](flecs::entity e, PositionComponent& phyiscal, RenderableComponent& renderable, VelocityComponent* vel, StateComponent* state)
		{
			Display& disp = _game.getDisplay();
			Renderer& renderer = disp.getRenderer();
			double aspect = disp.getAspectRatio();
			double zoom = disp.getZoom();
			double camX = disp.getCamX();
			double camY = disp.getCamY();

			double viewLeft = camX - (zoom * aspect);
			double viewRight = camX + (zoom * aspect);
			double viewTop = camY + zoom;
			double viewBottom = camY - zoom;

			auto& pos = phyiscal.pos;
			auto& sprite = renderable.sprite;
			
			//check if the object is inside the viewport
			if ((pos.getX() + sprite.getWidth() >= viewLeft) && (pos.getX() <= viewRight))
			{
				if ((pos.getY() + sprite.getHeight() >= viewBottom) && (pos.getY() <= viewTop))
				{
					double velocity = 0;
					if (vel != nullptr)
						velocity = vel->getData().getX();

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
					sprite.render(_game.getTextureManager(), renderer, pos, ObjectZ::MIDDLE, 0.f, hflip, vflip);

					// Update animations
					sprite.updateAnimation(e.delta_time(), velocity);
				}
			}
		});
}

hiage::PhysicsSystem::PhysicsSystem(double gravity) : System(), _gravity(gravity)
{
}

void hiage::PhysicsSystem::registerSystem(flecs::world& world)
{
	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<PhysicsComponent, VelocityComponent>();

	for (auto& t : componentTuples)
	{
		auto& physics = std::get<1>(t);
		auto& movement = std::get<2>(t);
		auto& vel = movement->getData();

		auto& physicsProps = physics->getData();
		if (physicsProps.hasGravity)
			vel.add(Vector2<double>(0, -1) * _gravity * frameTime);
		
		if (physicsProps.airResistance > 0 && vel.length() > 0)
			vel.subtract(vel.normalized() * physicsProps.airResistance);
	}*/
}


hiage::ControllerSystem::ControllerSystem() : System()
{
}

void hiage::ControllerSystem::registerSystem(flecs::world& world)
{
	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<ControllerComponent, ControllerStateComponent>();
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
	}*/
}

void hiage::ObjectObjectCollisionDetectionSystem::registerSystem(flecs::world& world)
{
	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<CollidableComponent, PositionComponent, VelocityComponent>();

	// Clear previous collisions
	for (auto& c : componentTuples)
		get<1>(c)->getData().objectCollisions.clear();

	// Sort by x coordinate
	gameState.getEntityManager().sortEntitiesByPosition();

	BoundingPolygon polygon1;

	vector<BoundingPolygon> tempVecPolygon2;
	tempVecPolygon2.resize(1);
	// Check for collisions
	for (int i = 0; i < componentTuples.size(); i++)
	{
		auto& c1 = componentTuples[i];
		auto entityId1 = get<0>(c1);
		auto& col1 = get<1>(c1)->getData();
		const auto& pos1 = get<2>(c1)->getData();
		const auto& vel1 = get<3>(c1)->getData();

		polygon1 = col1.boundingPolygon;

		polygon1.translate(pos1);

		for (int j = i + 1; j < componentTuples.size(); j++)
		{
			auto& c2 = componentTuples[j];
			auto entityId2 = get<0>(c2);
			auto& col2 = get<1>(c2)->getData();
			const auto& pos2 = get<2>(c2)->getData();
			auto relativeFrameVelocity = (vel1 - get<3>(c2)->getData()) * frameTime;
			tempVecPolygon2[0] = get<1>(c2)->getData().boundingPolygon;

			tempVecPolygon2[0].translate(pos2);

			// Distance check: If object 2 is further to the right than it's possible to move in one frame, we can skip this check.
			// For subsequent objects, they will be even more to the right, so we can break out early here.
			auto xDistance = tempVecPolygon2[0].getLeft() - polygon1.getRight();
			if (xDistance > relativeFrameVelocity.getX())
				break;

			for (int axis = 0; axis <= 1; axis++)
			{
				auto result = collisionTester.testCollision(polygon1, relativeFrameVelocity, tempVecPolygon2, axis);

				if (result.willIntersect || result.isIntersecting)
				{
					// Add two collision events - object 1 colliding with object 2, and the other way around.
					col1.objectCollisions.push_back(ObjectObjectCollisionData{
						.entityId1 = entityId1,
						.entityId2 = entityId2,
						.collisionResult = result
						});

					auto result2 = result;
					result2.hitNormal *= -1;
					col2.objectCollisions.push_back(ObjectObjectCollisionData{
						.entityId1 = entityId2,
						.entityId2 = entityId1,
						.collisionResult = result2
						});
				}
			}
		}
	}*/
}


SystemsManager::SystemsManager(flecs::world& ecs) : _ecs(ecs)
{
}

static int frameCounter = 0;
void hiage::ObjectTileCollisionDetectionSystem::registerSystem(flecs::world& world)
{
	/*frameCounter++;
	if (!tilemap.isLoaded())
		return;

	auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent>();

	for (auto& c : componentTuples)
	{
		const auto entityId = get<0>(c);
		const auto& pos = get<1>(c)->getData();
		const auto& vel = get<2>(c)->getData();
		auto& col = get<3>(c)->getData();
		col.tileCollisions.clear();

		Vector2<double> dvelocity = vel * frameTime;
		Vector2<double> currentPosition = pos;

		//get the collision box of the object
		BoundingPolygon objectPolygon = get<3>(c)->getData().boundingPolygon;
		objectPolygon.translate(currentPosition);

		// Get bounding polygons for tiles within the sprite's overlap
		vector<BoundingPolygon> tilePolygons = tilemap.getBoundingPolygonsInRect(objectPolygon.getLeft() + vel.getX() * frameTime, objectPolygon.getTop() + vel.getY() * frameTime, objectPolygon.getRight() + vel.getX() * frameTime, objectPolygon.getBottom() + vel.getY() * frameTime);
		for (int axis = 0; axis <= 1; axis++)
		{
			auto result = collisionTester.testCollision(objectPolygon, vel * frameTime, tilePolygons, axis);
			if (result.willIntersect || result.isIntersecting)
			{
				col.tileCollisions.push_back(ObjectTileCollisionData{
					.entityId = entityId,
					.collisionResult = result
					});
			}
		}
	}*/
}

void hiage::BlockingTileSystem::registerSystem(flecs::world& world)
{
	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, CollidableComponent>();
	for (auto& c : componentTuples)
	{
		auto& pos = std::get<1>(c)->getData();
		auto& vel = std::get<2>(c)->getData();
		auto& col = std::get<3>(c)->getData();
		
		for (auto& tc : col.tileCollisions)
		{
			auto& collisionResult = tc.collisionResult;

			// collisionTimeFactor is the fraction of the velocity that should be applied to move the object to the position of the collision
			auto collisionTimeFactor = frameTime * collisionResult.collisionTime;
			// Calculate the per-axis velocity
			Vec2d deltaPos(vel.getX() * collisionTimeFactor * collisionResult.axis.getX(), vel.getY() * collisionTimeFactor * collisionResult.axis.getY());
			pos.add(deltaPos - vel * 1.0e-6);

			// Adjust the velocity according to the hit normal
			vel.set(vel - (collisionResult.hitNormal * (1.0 + 0) * vel.dot(collisionResult.hitNormal)));
		}
	}*/
}

void hiage::AnimationSystem::registerSystem(flecs::world& world)
{
	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<RenderableComponent, StateComponent>();

	for (auto& t : componentTuples)
	{
		auto& renderable = std::get<1>(t);
		auto& state = std::get<2>(t)->getData();
		auto& sprite = renderable->getData();

		sprite.playAnimation(state.stateName, false);
	}*/
}

void hiage::ObjectTrackingSystem::registerSystem(flecs::world& world)
{
	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<PositionComponent, VelocityComponent, TrackableComponent>();

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
	}*/
}

void hiage::CameraSystem::registerSystem(flecs::world& world)
{
	world.system<PositionComponent, CameraComponent>()
		.each([&](flecs::entity e, PositionComponent& physical, CameraComponent& camera)
		{
			
			auto& display = _game.getDisplay();
			auto& pos = physical.pos;
			auto& camProps = camera.getData();

			// TODO - Make boundaries configurable.
			display.setZoom(camProps.zoom);

			auto zoom = display.getZoom();
			auto aspect = display.getAspectRatio();
			auto leftBoundary = zoom * aspect;
			auto bottomBoundary = zoom;


			display.setCamPosition(std::max(pos.x, leftBoundary), std::max(pos.y, bottomBoundary));
	});
}
