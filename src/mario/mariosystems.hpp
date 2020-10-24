#pragma once

#include <hiage/core/systems.hpp>
#include <hiage/core/game.hpp>

class CharacterStateMachineSystem : public hiage::System
{
private:
	void handleCollisions();
	void updateState();
public:
	CharacterStateMachineSystem(hiage::Game& game, hiage::GameState& gameState);
	virtual void update(double frameTime) override;

};

class CharacterControllerSystem : public hiage::System
{
public:
	CharacterControllerSystem(hiage::Game& game, hiage::GameState& gameState);
	virtual void update(double frameTime) override;
};

class MarioCollisionResponseSystem : public hiage::System
{
public:
	MarioCollisionResponseSystem(hiage::Game& game, hiage::GameState& gameState);
	virtual void update(double frameTime) override;
};

class AISystem : public hiage::System
{
public:
	AISystem(hiage::Game& game, hiage::GameState& gameState);
	virtual void update(double frameTime) override;
};

