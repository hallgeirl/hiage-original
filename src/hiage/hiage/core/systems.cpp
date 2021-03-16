#include "systems.hpp"
#include "game.hpp"
#include "quadtree.hpp"
#include <SDL/SDL.h>
#include <algorithm>
#include <tuple>
#include <cstdlib>

using namespace std;
using namespace hiage;


// TODO flecs

System::System()
{
}

System::~System()
{
}

DebugSystem::DebugSystem(Game& game) : System(), _game(game)
{
}

void DebugSystem::registerSystem(flecs::world& world)
{
	world.system<PositionComponent, NameComponent>()
		.each([&](flecs::entity e, PositionComponent& pos, NameComponent& name)
		{
			const double margin = 100.;
			auto viewPort = _game.getDebugRenderer()->getViewPort();
			if (pos.pos.x > viewPort.left - margin && pos.pos.x < viewPort.right + margin && pos.pos.y > viewPort.bottom - margin && pos.pos.y < viewPort.top + margin)
			{ 
				stringstream ss;
				ss << "Entity(" << name.name << "): " << e.id();
				srand(name.name.length()*1000);
				auto yoffs = (float) rand() / RAND_MAX;
				_game.getDebugRenderer()->renderText(ss.str(), pos.pos.x, pos.pos.y - 20*yoffs);
			}
		});
}

MovementSystem::MovementSystem() : System()
{
}

void MovementSystem::registerSystem(flecs::world& world)
{
	world.system<PositionComponent, VelocityComponent>()
		.each([](flecs::entity e, PositionComponent& position, VelocityComponent& velocity)
		{
			auto vel = velocity.vel;
			
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
			auto& spriteController = _game.getSpriteController();
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
						velocity = vel->vel.x;

					bool hflip = false, vflip = false;
					if (state != nullptr)
					{
						auto& metadata = state->metadata;
						if (metadata.contains("x-flip") && get<int>(metadata.at("x-flip")) != 0)
							hflip = true;
						if (metadata.contains("y-flip") && get<int>(metadata.at("y-flip")) != 0)
							vflip = true;
					}

					// Render sprite
					spriteController.render(sprite, pos, ObjectZ::MIDDLE, 0.f, hflip, vflip);

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
	world.system<PhysicsComponent, VelocityComponent>()
		.each([&](flecs::entity e, PhysicsComponent& physics, VelocityComponent& velocity)
		{
			if (physics.hasGravity)
				velocity.vel.add(Vector2<double>(0, -1) * _gravity * e.delta_time());
			
			if (physics.airResistance > 0 && velocity.vel.length() > 0)
				velocity.vel.subtract(velocity.vel.normalized() * physics.airResistance);
		});
}


hiage::ControllerSystem::ControllerSystem(Game& game) : System(), _game(game)
{
}

void hiage::ControllerSystem::registerSystem(flecs::world& world)
{
	world.system<ControllerComponent, ControllerStateComponent>()
		.each([&](flecs::entity e, ControllerComponent& controller, ControllerStateComponent& controllerState)
		{
			if (controller.controllerType == "human")
			{
				auto& inputManager = _game.getInputManager();
				auto actions = inputManager.getControllerActions();
				controllerState.controllerState = actions;
			}
			else if (controller.controllerType == "constant")
			{
				std::unordered_set<std::string> actions;
				for (auto& a : controller.constantActions)
					actions.insert(a);

				controllerState.controllerState = actions;
			}
		});
}

void hiage::ObjectObjectCollisionDetectionSystem::registerSystem(flecs::world& world)
{
	world.system<>()
		.iter([&](flecs::iter&) {
			// Create a square quadtree by taking the largest dimension and squaring it
			const int gridSize = 32;
			auto width = _tileMap.getWidth() * _tileMap.getTileSize();
			auto height = _tileMap.getHeight() * _tileMap.getTileSize();

			if (!_grid.initialized())
				_grid.init(BoundingBox(0, 0, width, height), 32, _game.getDebugRenderer());
		});

	world.system<CollidableComponent, PositionComponent, VelocityComponent*>()
		.each([&](flecs::entity e, CollidableComponent& collidable, PositionComponent& position, VelocityComponent* velocity)
		{
			auto poly = collidable.boundingPolygon;
			poly.translate(position.pos);
			auto bb = BoundingBox<int32_t>(poly.getLeft(), poly.getBottom(), poly.getRight(), poly.getTop());
			if (velocity != nullptr)
			{
				if (velocity->vel.x > 0)
					bb.right += velocity->vel.x * e.delta_time();
				else
					bb.left += velocity->vel.x * e.delta_time();

				if (velocity->vel.y > 0)
					bb.top += velocity->vel.y * e.delta_time();
				else
					bb.bottom += velocity->vel.y * e.delta_time();
			}
			_grid.insert(e.id(), bb);
		});

	world.system<CollidableComponent, PositionComponent, VelocityComponent>()
		.each([&](flecs::entity e, CollidableComponent& collidable, PositionComponent& position, VelocityComponent& velocity)
		{
			collidable.objectCollisions.clear();

			vector<BoundingPolygon> tempVecPolygon2;
			tempVecPolygon2.resize(1);
			// Check for collisions
			auto entityId1 = e.id();
			auto& col1 = collidable;
			const auto& pos1 = position.pos;
			const auto& vel1 = velocity.vel;

			BoundingPolygon polygon1 = col1.boundingPolygon;
			polygon1.translate(pos1);

			auto candidates = _grid.getElementsNear(entityId1);

			for (auto entityId2 : candidates)
			{
				if (entityId2 == entityId1)
					continue;

				flecs::entity c2(e.world(), entityId2);
				
				auto col2 = c2.get<CollidableComponent>();
				const auto pos2 = c2.get<PositionComponent>()->pos;
				auto vel2Comp = c2.get<VelocityComponent>();
				Vec2d vel2(0,0);
				if (vel2Comp != nullptr)
					vel2 = vel2Comp->vel;

				auto relativeFrameVelocity = (vel1 - vel2) * e.delta_time();
				tempVecPolygon2[0] = col1.boundingPolygon;

				tempVecPolygon2[0].translate(pos2);

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
					}
				}
			}
		});

	// Debugging system
	world.system<>()
		.iter([&](flecs::iter&) {
		auto debugWriter = _game.getDebugRenderer();
		_grid.renderDebugInfo();
	});

	world.system<CollidableComponent, PositionComponent>()
		.each([&](flecs::entity, CollidableComponent& col, PositionComponent& pos) {
			DebugRenderer* debugWriter = _game.getDebugRenderer();
			if (debugWriter->enabled())
			{ 
				if (debugWriter->getDebugFlags().collisionDetection.showObjectCollisions && col.objectCollisions.size() > 0)
				{
					debugWriter->renderText("Collided!", pos.pos.x, pos.pos.y);
				}

				if (debugWriter->getDebugFlags().collisionDetection.drawBoundingPolygon)
				{
					BoundingPolygon polygon = col.boundingPolygon;
					polygon.translate(pos.pos);
					auto vertices = polygon.getVertices();
					vertices.push_back(vertices[0]); // wrap around to the first vertex
					debugWriter->renderLines(vertices);
				}
			}
		});
}



static int frameCounter = 0;
void hiage::ObjectTileCollisionDetectionSystem::registerSystem(flecs::world& world)
{
	frameCounter++;
	//if (!tilemap.isLoaded())
		//return;

	world.system<PositionComponent, VelocityComponent, CollidableComponent>()
		.each([&](flecs::entity e, PositionComponent& position, VelocityComponent& velocity, CollidableComponent& collidable)
		{
			const auto& pos = position.pos;
			const auto& vel = velocity.vel;
			
			collidable.tileCollisions.clear();

			Vector2<double> dvelocity = vel * e.delta_time();
			Vector2<double> currentPosition = pos;

			//get the collision box of the object
			BoundingPolygon objectPolygon = collidable.boundingPolygon;
			objectPolygon.translate(currentPosition);

			// Get bounding polygons for tiles within the sprite's overlap
			vector<BoundingPolygon> tilePolygons = tilemap.getBoundingPolygonsInRect(objectPolygon.getLeft() + vel.getX() * e.delta_time(), 
																					 objectPolygon.getTop() + vel.getY() * e.delta_time(), 
																					 objectPolygon.getRight() + vel.getX() * e.delta_time(), 
																					 objectPolygon.getBottom() + vel.getY() * e.delta_time());
			for (int axis = 0; axis <= 1; axis++)
			{
				auto result = collisionTester.testCollision(objectPolygon, vel * e.delta_time(), tilePolygons, axis);
				if (result.willIntersect || result.isIntersecting)
				{
					collidable.tileCollisions.push_back(ObjectTileCollisionData{
						.entityId = e.id(),
						.collisionResult = result
					});
				}
			}
		});
}

void hiage::BlockingTileSystem::registerSystem(flecs::world& world)
{
	world.system<PositionComponent, VelocityComponent, CollidableComponent>()
		.each([&](flecs::entity e, PositionComponent& position, VelocityComponent& velocity, CollidableComponent& collidable)
		{
			
			auto& pos = position.pos;
			auto& vel = velocity.vel;
			auto& col = collidable;
			
			for (auto& tc : col.tileCollisions)
			{
				auto& collisionResult = tc.collisionResult;

				// collisionTimeFactor is the fraction of the velocity that should be applied to move the object to the position of the collision
				auto collisionTimeFactor = e.delta_time() * collisionResult.collisionTime;
				// Calculate the per-axis velocity
				Vec2d deltaPos(vel.getX() * collisionTimeFactor * collisionResult.axis.getX(), vel.getY() * collisionTimeFactor * collisionResult.axis.getY());
				pos.add(deltaPos - vel * 1.0e-6);

				// Adjust the velocity according to the hit normal
				vel.set(vel - (collisionResult.hitNormal * (1.0 + 0) * vel.dot(collisionResult.hitNormal)));
			}
		});
}

void hiage::AnimationSystem::registerSystem(flecs::world& world)
{
	world.system<RenderableComponent, StateComponent>()
		.each([&](flecs::entity, RenderableComponent& renderable, StateComponent& state)
		{
			renderable.sprite.playAnimation(state.stateName, false);
		});
}

void hiage::ObjectTrackingSystem::registerSystem(flecs::world& world)
{
	world.system<>()
		.iter([&](const flecs::iter&) {
            _trackingTargets.clear();
        });

	// Step 1: Make a list of trackable objects
	world.system<PositionComponent, VelocityComponent, TrackableComponent>()
		.each([&](flecs::entity e, PositionComponent& position, VelocityComponent& velocity, TrackableComponent&)
		{
			_trackingTargets.push_back(std::make_tuple(position, velocity));
		});

	world.system<PositionComponent, VelocityComponent, TrackingComponent>()
		.each([&](flecs::entity e, PositionComponent& trackingObjectPosition, VelocityComponent& trackingObjectVelocity, TrackingComponent& tracking)
		{
			if (_trackingTargets.size() == 0)
				return;

			//TODO: This is not great - only supporting a single tracking target for now. But for my use, it's fine. For now.
			auto& trackingTargetPosition = std::get<0>(_trackingTargets[0]).pos;
			auto& trackingTargetVelocity = std::get<1>(_trackingTargets[0]).vel;

			std::string mode = "elastic";
			if (tracking.mode.length() > 0)
				mode = tracking.mode;
			
			if (mode == "fixed")
			{
				trackingObjectPosition.pos.set(trackingTargetPosition);
			} 
			else if (mode == "elastic")
			{
				auto diff = (trackingTargetPosition + trackingTargetVelocity) - trackingObjectPosition.pos;

				trackingObjectVelocity.vel.scale(0.5);
				auto accelVector = diff * 100. + trackingTargetVelocity;
				trackingObjectVelocity.vel.add(accelVector * e.delta_time());
			}
		});
}

void hiage::CameraSystem::registerSystem(flecs::world& world)
{
	world.system<PositionComponent, CameraComponent>()
		.each([&](flecs::entity, PositionComponent& physical, CameraComponent& camera)
		{
			
			auto& display = _game.getDisplay();
			auto& pos = physical.pos;

			// TODO - Make boundaries configurable.
			display.setZoom(camera.zoom);

			auto zoom = display.getZoom();
			auto aspect = display.getAspectRatio();
			auto leftBoundary = zoom * aspect;
			auto bottomBoundary = zoom;


			display.setCamPosition(std::max(pos.x, leftBoundary), std::max(pos.y, bottomBoundary));
	});
}

hiage::DebugWriterRenderingSystem::DebugWriterRenderingSystem(Game& game, Font& font) : System(), _game(game), _font(font)
{
}

void hiage::DebugWriterRenderingSystem::registerSystem(flecs::world& world)
{
	world.system<>()
		.iter([&](flecs::iter&)
		{
			/*auto debugWriter = _game.getDebugRenderer();
			int spacingX = 100;
			int spacingY = _font.getCharacterHeight()*0.15;
			int yOffs = 0, xOffs = 0;
			
			_game.printTextFixed(_font, "DEBUG LOG", -200 + xOffs, spacingY * 2 - 50, ScreenHorizontalPosition::Right, ScreenVerticalPosition::Top, 0.2, -0.2);
			_game.printTextFixed(_font, "=========", -200 + xOffs, spacingY - 50, ScreenHorizontalPosition::Right, ScreenVerticalPosition::Top, 0.2, -0.2);
			*/
			/*for (auto& s : debugWriter.getBuffer())
			{
				_game.printTextFixed(_font, s, -200 + xOffs, -yOffs - 50, ScreenHorizontalPosition::Right, ScreenVerticalPosition::Top, 0.2, -0.2);
				//_game.printTextFixed(_font, s, 0, -yOffs+500, ScreenHorizontalPosition::Center, ScreenVerticalPosition::Center, 0.2, -0.2);
				yOffs += spacingY;

				if (yOffs > 500)
				{
					yOffs = 0;
					xOffs += spacingX;
				}
			}
			
			debugWriter.reset();*/
		});
}