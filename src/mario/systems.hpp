#pragma once

#include <hiage/core/systems.hpp>
#include <hiage/core/game.hpp>

class CharacterStateMachineSystem : public hiage::System
{
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