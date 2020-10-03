#include "systems.hpp"

using namespace std;
using namespace hiage;

CharacterStateMachineSystem::CharacterStateMachineSystem(Game& game, GameState& gameState) : System(game, gameState)
{
}

void CharacterStateMachineSystem::update(double)
{
	auto componentTuples = gameState.getEntityManager().queryComponentGroup<VelocityComponent, StateComponent>();

	for (auto& c : componentTuples)
	{
		auto& vel = get<1>(c)->getData();
		auto& state = get<2>(c)->getData();
		auto& metadata = state.metadata;

		if (vel.getX() < 0)
			metadata["x-flip"] = 1;
		else if (vel.getX() > 0)
			metadata["x-flip"] = 0;

		if (abs(vel.getX()) > 150)
		{
			state.stateName = "run";
		}
		else if (abs(vel.getX()) > 10)
		{
			state.stateName = "walk";
		}
		else
			state.stateName = "stand";

	}

}
