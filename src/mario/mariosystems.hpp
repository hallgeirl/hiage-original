#pragma once

#include <hiage/core/systems.hpp>
#include <hiage/core/game.hpp>

class CharacterStateMachineSystem : public hiage::System
{
private:
	void handleCollisions();
	void updateState();
public:
	CharacterStateMachineSystem();
	virtual void registerSystem(flecs::world& world) override;

};

class CharacterControllerSystem : public hiage::System
{
private:
	hiage::Game& _game;
public:
	CharacterControllerSystem(hiage::Game& game);
	virtual void registerSystem(flecs::world& world) override;
};

class MarioCollisionResponseSystem : public hiage::System
{
public:
	MarioCollisionResponseSystem();
	virtual void registerSystem(flecs::world& world) override;
};

class AISystem : public hiage::System
{
public:
	AISystem();
	virtual void registerSystem(flecs::world& world) override;
};

