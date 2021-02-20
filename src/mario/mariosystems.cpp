#include "mariosystems.hpp"
#include "mariocomponents.hpp"
#include "events.hpp"

using namespace std;
using namespace hiage;

void CharacterStateMachineSystem::handleCollisions()
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<CollidableComponent, StateComponent>();

	for (auto& c : componentTuples)
	{
		auto& col = get<1>(c)->getData();
		auto& state = get<2>(c)->getData();

		for (auto& tc : col.tileCollisions)
		{
			if (tc.collisionResult.hitNormal.getY() > 0.5) {
				auto& metadata = state.metadata;
				metadata["onGround"] = 1;
				metadata["ticks-since-landed"] = 0;
			}
		}
	}
}

void CharacterStateMachineSystem::updateState()
{
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

CharacterStateMachineSystem::CharacterStateMachineSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void CharacterStateMachineSystem::update(double)
{
	handleCollisions();
	updateState();

}

CharacterControllerSystem::CharacterControllerSystem(hiage::Game& game, hiage::GameState& gameState) : System(game, gameState)
{
}

void CharacterControllerSystem::update(double frameTime)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<VelocityComponent, ControllerStateComponent, StateComponent, SpeedLimitComponent>();

	double magnitude = 800. * frameTime;

	for (auto& c : componentTuples)
	{
		auto& vel = get<1>(c)->getData();
		auto& controllerState = get<2>(c)->getData();
		auto& state = get<3>(c)->getData();
		auto& speedlimit = get<4>(c)->getData();

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
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<CollidableComponent, PositionComponent, VelocityComponent>();

	for (auto& c1 : componentTuples)
	{
		auto& col = get<1>(c1)->getData();
		auto& pos = get<2>(c1)->getData();
		auto& vel = get<3>(c1)->getData();

		for (auto& oc : col.objectCollisions)
		{
			// Handle moving object colliding with blocking object
			auto c2 = gameState.getEntityManager().queryComponentGroup<CollidableComponent, BlockingComponent>(oc.entityId2);

			if (get<0>(c2) != nullptr)
			{
				auto& collisionResult = oc.collisionResult;

				// collisionTimeFactor is the fraction of the velocity that should be applied to move the object to the position of the collision
				auto collisionTimeFactor = frameTime * collisionResult.collisionTime;
				// Calculate the per-axis velocity
				Vec2d deltaPos(vel.getX() * collisionTimeFactor * collisionResult.axis.getX(), vel.getY() * collisionTimeFactor * collisionResult.axis.getY());
				pos.add(deltaPos - vel * 1.0e-6);

				// Adjust the velocity according to the hit normal
				vel.set(vel - (collisionResult.hitNormal * (1.0 + 0) * vel.dot(oc.collisionResult.hitNormal)));

				auto state = gameState.getEntityManager().queryComponentGroup<StateComponent>(oc.entityId1);

				// normal vector of 30 degrees or higher => count as solid ground
				if (oc.collisionResult.hitNormal.getY() > 0.5 && state != nullptr) {
					auto& metadata = state->getData().metadata;
					metadata["onGround"] = 1;
					metadata["ticks-since-landed"] = 0;
				}
			}

		}
	}
}

AISystem::AISystem(hiage::Game& game, hiage::GameState& gameState) : System(game, gameState)
{
}


void AISystem::update(double)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<CollidableComponent, GroundMonsterControllerComponent, ControllerStateComponent, VelocityComponent>();

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
	}
}
