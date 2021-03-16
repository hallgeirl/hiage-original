#include "mariosystems.hpp"
#include "mariocomponents.hpp"
#include "events.hpp"

using namespace std;
using namespace hiage;

CharacterStateMachineSystem::CharacterStateMachineSystem() : System()
{
}

void CharacterStateMachineSystem::registerSystem(flecs::world& world)
{
	world.system<CollidableComponent, StateComponent>()
		.each([](flecs::entity, CollidableComponent& collidable, StateComponent& state) 
		{
			for (auto& tc : collidable.tileCollisions)
			{
				if (tc.collisionResult.hitNormal.getY() > 0.5) {
					auto& metadata = state.metadata;
					metadata["onGround"] = 1;
					metadata["ticks-since-landed"] = 0;
				}
			}	
		});

	world.system<VelocityComponent, StateComponent>()
		.each([](flecs::entity, VelocityComponent& velocity, StateComponent& state) 
		{
			auto& vel = velocity.vel;
			auto& metadata = state.metadata;

			if (vel.getX() < -1e-6)
				metadata["x-flip"] = 1;
			else if (vel.getX() > 1e-6)
				metadata["x-flip"] = 0;

			if (!metadata.contains("ticks-since-landed"))
				metadata["ticks-since-landed"] = 0;

			std::get<int>(metadata["ticks-since-landed"])++;

			if (get<int>(metadata["ticks-since-landed"]) > 5)
			{
				metadata["onGround"] = 0;
				if (vel.getY() > 0)
				{
					if (abs(vel.getX()) > 150)
						state.stateName = "longjump";
					else
						state.stateName = "jump";

				}
				else
					state.stateName = "fall";
			}
			else
			{
				if (abs(vel.getX()) > 150)
				{
					state.stateName = "run";
				}
				else if (abs(vel.getX()) > 10)
				{
					state.stateName = "walk";
				}
				else
				{
					state.stateName = "stand";
				}
			}
		});
}

CharacterControllerSystem::CharacterControllerSystem(Game& game) : System(), _game(game)
{
}

void CharacterControllerSystem::registerSystem(flecs::world& world)
{
	world.system<VelocityComponent, ControllerStateComponent, StateComponent, SpeedLimitComponent>()
		.each([&](flecs::entity e, VelocityComponent& velocity, ControllerStateComponent& controllerStateComponent, StateComponent& state, SpeedLimitComponent& speedlimit)
		{
			double magnitude = 800. * e.delta_time();

			auto& vel = velocity.vel;
			auto& controllerState = controllerStateComponent.controllerState;
			speedlimit.speedLimit.setX(110);

			// apply slowdown
			if (abs(vel.getX()) > 400 * e.delta_time() && state.metadata.contains("onGround"))
			{
				if (vel.getX() > 0)
					vel.add(Vector2<double>(-400. * e.delta_time(), 0));
				if (vel.getX() < 0)
					vel.add(Vector2<double>(400. * e.delta_time(), 0));
			}
			else
			{
				vel.setX(0);
			}

			if (controllerState.contains("run"))
				speedlimit.speedLimit.setX(220);

			if (controllerState.contains("goRight") && abs(vel.getX()) < speedlimit.speedLimit.getX())
				vel.add(Vector2<double>(1, 0) * magnitude);
			else if (controllerState.contains("goLeft") && abs(vel.getX()) < speedlimit.speedLimit.getX())
				vel.add(Vector2<double>(-1, 0) * magnitude);
			else if (controllerState.contains("crouch"))
				vel.add(Vector2<double>(0, -1) * magnitude);
			else if (controllerState.contains("lookUp"))
				vel.add(Vector2<double>(0, 1) * magnitude);

			if (controllerState.contains("jump") && state.metadata.contains("onGround") && get<int>(state.metadata.at("onGround")) != 0)
			{
				_game.getAudioManager().playWav("NormalJump");
				state.metadata["onGround"] = 0;
				vel.setY(300);

			}
		});

}

MarioCollisionResponseSystem::MarioCollisionResponseSystem() : System()
{
}

void MarioCollisionResponseSystem::registerSystem(flecs::world& world)
{
	world.system<CollidableComponent, PositionComponent, VelocityComponent, StateComponent*>()
		.each([](flecs::entity e, CollidableComponent& collidable, PositionComponent& position, VelocityComponent& velocity, StateComponent* state) 
		{
			auto& col = collidable;
			auto& pos = position.pos;
			auto& vel = velocity.vel;

			for (auto& oc : col.objectCollisions)
			{
				// Handle moving object colliding with blocking object
				auto e2 = flecs::entity(e.world(), oc.entityId2);
				auto blocking = e2.get<BlockingComponent>();

				if (blocking != nullptr)
				{
					auto& collisionResult = oc.collisionResult;

					// collisionTimeFactor is the fraction of the velocity that should be applied to move the object to the position of the collision
					auto collisionTimeFactor = e.delta_time() * collisionResult.collisionTime;
					// Calculate the per-axis velocity
					Vec2d deltaPos(vel.getX() * collisionTimeFactor * collisionResult.axis.getX(), vel.getY() * collisionTimeFactor * collisionResult.axis.getY());
					pos.add(deltaPos - vel * 1.0e-6);

					// Adjust the velocity according to the hit normal
					vel.set(vel - (collisionResult.hitNormal * (1.0 + 0) * vel.dot(oc.collisionResult.hitNormal)));

					// normal vector of 30 degrees or higher => count as solid ground
					if (oc.collisionResult.hitNormal.getY() > 0.5 && state != nullptr) {
						auto& metadata = state->metadata;
						metadata["onGround"] = 1;
						metadata["ticks-since-landed"] = 0;
					}
				}

			}
		});
}

AISystem::AISystem() : System()
{
}


void AISystem::registerSystem(flecs::world& world)
{
	/*auto componentTuples = gameState.getEntityManager().queryComponentGroup<CollidableComponent, GroundMonsterControllerComponent, ControllerStateComponent, VelocityComponent>();

	for (auto& c : componentTuples)
	{
		auto& col = get<1>(c)->getData();
		auto& gmc = get<2>(c)->getData();
		auto& controllerState = get<3>(c);
		auto& vel = get<4>(c)->getData();

		for (auto& tc : col.tileCollisions)
		{
			if (tc.collisionResult.hitNormal.getX() > 0.7)
				gmc.direction = "right";
			else if (tc.collisionResult.hitNormal.getX() < -0.7)
				gmc.direction = "left";
		}

		for (auto& oc : col.objectCollisions)
		{
			if (oc.collisionResult.hitNormal.getX() > 0.7)
			{
				gmc.direction = "right";
				vel.setX(vel.getX() * -1);
			}
			else if (oc.collisionResult.hitNormal.getX() < -0.7)
			{
				gmc.direction = "left";
				vel.setX(vel.getX() * -1);
			}
		}

		unordered_set<string> actions;

		if (gmc.direction == "left")
			actions.insert("goLeft");
		else if (gmc.direction == "right")
			actions.insert("goRight");

		controllerState->setData(actions);
	}*/
}
