#pragma once

#include <string>
#include <memory>

namespace hiage
{
	class Game;
	class GameState;
	class Tilemap;

	/*
		Systems
	*/
	class System
	{
	protected:
		Game& game;
		GameState& gameState;

	public:
		System(Game& game, GameState& gameState);
		virtual ~System();
		virtual void update(double frameTime) = 0;
	};

	class MovementSystem : public System
	{
	public:
		MovementSystem(Game& game, GameState& gameState);
		virtual void update(double frametime) override;
	};

	class ObjectRenderingSystem : public System
	{
	public:
		ObjectRenderingSystem(Game& game, GameState& gameState);
		virtual void update(double frameTime) override;
	};

	class GravitySystem : public System
	{
	private:
		double magnitude = 30;

	public:
		GravitySystem(Game& game, GameState& gameState);
		virtual void update(double frameTime) override;
	};

	class HumanControllerSystem : public System
	{
	public:
		HumanControllerSystem(Game& game, GameState& gameState);
		virtual void update(double frameTime) override;
	};

	class ObjectObjectCollisionSystem : public System
	{
	public:
		ObjectObjectCollisionSystem(Game& game, GameState& gameState) : System(game, gameState) {}
		virtual void update(double frameTime) override;
	};

	class ObjectTileCollisionSystem : public System
	{
	private:
		const Tilemap& tilemap;
	public:
		ObjectTileCollisionSystem(Game& game, GameState& gameState, const Tilemap& tilemap) : System(game, gameState), tilemap(tilemap) {}
		virtual void update(double frameTime) override;
	};

	template<typename ...TAll>
	struct PositionComponentComparator {
		bool operator()(std::tuple<std::shared_ptr<TAll>...> a,
						std::tuple<std::shared_ptr<TAll>...> b) const
		{
			return get<0>(a)->getData().getX() < get<0>(b)->getData().getX();
		}
	};


	// List of currently missing systems:
	// - Script system??
	// - Collision
	// - Object destruction?
	// - Controllers
	/*
		SystemsFactory
	*/
	class SystemsFactory
	{
	private:
		Game& game;
		GameState& gameState;
	public:
		SystemsFactory(Game& game, GameState& gameState);
		
		template<typename T, typename ...TRest>
		std::unique_ptr<System> createSystem(TRest... args)
		{ 
			return make_unique<T>(game, gameState, args...);
		};
	};
}