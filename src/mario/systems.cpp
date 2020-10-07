#include "systems.hpp"

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
			auto& state = c->getData();

			// normal vector of 30 degrees or higher => count as solid ground
			if (myEvt.getData().normalVector.getY() > 0.5) {
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
