#include "systems.hpp"
#include "components.hpp"
#include "events.hpp"

using namespace std;
using namespace hiage;

CharacterStateMachineSystem::CharacterStateMachineSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void CharacterStateMachineSystem::update(double)
{
	auto& events = gameState.getEventQueue().peekAll(BuiltinEventTypes::Collision_ObjectTile);

	for (auto& evt : events)
	{
		auto& myEvt = dynamic_cast<ObjectTileCollisionEvent&>(*evt);

		auto c = gameState.getEntityManager().queryComponentGroup<StateComponent>(myEvt.getData().entityId);
		if (c != nullptr)
		{
			//auto& state = c->getData();

			// normal vector of 30 degrees or higher => count as solid ground
			if (myEvt.getData().collisionResult.hitNormal.getY() > 0.5) {
				auto& metadata = c->getData().metadata;
				metadata["onGround"] = 1;
				metadata["ticks-since-landed"] = 0;
			}
		}
	}

	auto componentTuples = gameState.getEntityManager().queryComponentGroup<VelocityComponent, StateComponent>();

	for (auto& c : componentTuples)
	{
		auto& vel = get<1>(c)->getData();
		auto& state = get<2>(c)->getData();
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
	}
}

CharacterControllerSystem::CharacterControllerSystem(hiage::Game& game, hiage::GameState& gameState) : System(game, gameState)
{
}

void CharacterControllerSystem::update(double frameTime)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<VelocityComponent, ControllerStateComponent, StateComponent, SpeedLimitComponent, PositionComponent>();

	double magnitude = 800. * frameTime;

	for (auto& c : componentTuples)
	{
		auto& vel = get<1>(c)->getData();
		auto& controllerState = get<2>(c)->getData();
		auto& state = get<3>(c)->getData();
		auto& speedlimit = get<4>(c)->getData();
		auto& pos = get<5>(c)->getData();

		cout << "pos = " << pos.getX() << "," << pos.getY() << endl;
		
		speedlimit.speedLimit.setX(110);

		// apply slowdown
		if (abs(vel.getX()) > 400 * frameTime && state.metadata.contains("onGround"))
		{
			if (vel.getX() > 0)
				vel.add(Vector2<double>(-400. * frameTime, 0));
			if (vel.getX() < 0)
				vel.add(Vector2<double>(400. * frameTime, 0));
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
			game.getAudioManager().playWav("NormalJump");
			state.metadata["onGround"] = 0;
			vel.setY(300);

		}
	}
}

MarioCollisionResponseSystem::MarioCollisionResponseSystem(hiage::Game& game, hiage::GameState& gameState) : System(game, gameState)
{
}

void MarioCollisionResponseSystem::update(double frameTime)
{
	auto& events = gameState.getEventQueue().peekAll(BuiltinEventTypes::Collision_ObjectObject);

	for (auto& evt : events)
	{
		auto& myEvt = dynamic_cast<ObjectObjectCollisionEvent&>(*evt);

		// Handle moving object colliding with blocking object
		auto c1 = gameState.getEntityManager().queryComponentGroup<CollidableComponent, PositionComponent, VelocityComponent>(myEvt.getData().entityId1);
		auto c2 = gameState.getEntityManager().queryComponentGroup<CollidableComponent, BlockingComponent>(myEvt.getData().entityId2);
	
		auto& pos = get<1>(c1)->getData();
		auto& vel = get<2>(c1)->getData();

		if (get<0>(c2) != nullptr)
		{
			auto& collisionResult = myEvt.getData().collisionResult;

			// collisionTimeFactor is the fraction of the velocity that should be applied to move the object to the position of the collision
			auto collisionTimeFactor = frameTime * collisionResult.collisionTime;
			// Calculate the per-axis velocity
			Vec2d deltaPos(vel.getX() * collisionTimeFactor * collisionResult.axis.getX(), vel.getY() * collisionTimeFactor * collisionResult.axis.getY());
			pos.add(deltaPos - vel * 1.0e-6);

			// Adjust the velocity according to the hit normal
			vel.set(vel - (collisionResult.hitNormal * (1.0 + 0) * vel.dot(myEvt.getData().collisionResult.hitNormal)));

			auto state = gameState.getEntityManager().queryComponentGroup<StateComponent>(myEvt.getData().entityId1);

			// normal vector of 30 degrees or higher => count as solid ground
			if (myEvt.getData().collisionResult.hitNormal.getY() > 0.5 && state != nullptr) {
				auto& metadata = state->getData().metadata;
				metadata["onGround"] = 1;
				metadata["ticks-since-landed"] = 0;
			}
		}
	}
}
