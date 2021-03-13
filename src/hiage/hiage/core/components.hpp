#pragma once

#include "../util/vector2.h"
#include "../gfx/sprite.h"
#include "collisions.hpp"
#include "resourcedescriptors.hpp"

#include <flecs.h>

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <variant>

namespace hiage 
{
	/*
		Components.
		All component types must have a registered factory using ComponentManager::addGenericComponentFactory.
	*/

	struct DebugStateComponent
	{
		bool dumpEntities = false;

		bool drawQuadTree = true;
	};

	struct NameComponent
	{
		std::string name;
	};

	struct PositionComponent
	{
		Vec2d pos;

		PositionComponent() : pos(0, 0) { }
		PositionComponent(const ComponentProperties& properties);
	};

	struct VelocityComponent
	{
		Vec2d vel;

		VelocityComponent() : vel(0, 0) { }
		VelocityComponent(const ComponentProperties& properties);
	};

	struct RenderableComponent
	{
		Sprite sprite;
	};

	struct PhysicsComponent
	{
		double airResistance, groundFriction;
		bool hasGravity;

		PhysicsComponent() : airResistance(0), groundFriction(0), hasGravity(false) {}
		PhysicsComponent(const ComponentProperties& properties);
	};

	struct ControllerComponent
	{
		std::string controllerType;
		std::vector<std::string> constantActions;
		
		ControllerComponent() {}
		ControllerComponent(const ComponentProperties& properties);
	};

	/*
	Collision events
	*/
	struct ObjectTileCollisionData
	{
		uint64_t entityId;
		CollisionResult collisionResult;
	};
	
	struct ObjectObjectCollisionData
	{
		uint64_t entityId1, entityId2;
		CollisionResult collisionResult;
	};

	struct CollidableComponent
	{
		BoundingPolygon boundingPolygon;
		std::vector<ObjectObjectCollisionData> objectCollisions;
		std::vector<ObjectTileCollisionData> tileCollisions;

		CollidableComponent() {}
		CollidableComponent(const ComponentProperties& properties);
	};

	struct TrackingComponent
	{
		std::string mode = "fixed";

		TrackingComponent() {}
		TrackingComponent(const ComponentProperties& properties);
	};

	struct CameraComponent
	{
		double zoom = 400;
		CameraComponent() : zoom(400) {}
		CameraComponent(const ComponentProperties& properties);
	};

	// Used for "object state" when it comes to animations (e.g. "on ground", "standing", "walking", "jumping", "falling") and allowed actions (e.g. jumping is allowed when standing, etc.)
	// The actual rules are defined by the system that handles this component. This is just the container of the state name and metadata attached to the state.
	struct StateComponent
	{
		std::string stateName;
		std::unordered_map<std::string, std::variant<std::string, int, double>> metadata;

		StateComponent() {}
		StateComponent(const ComponentProperties& properties);
	};

	struct ControllerStateComponent
	{
		std::unordered_set<std::string> controllerState;
	};

	struct SpeedLimitComponent
	{
		Vec2d speedLimit;

		SpeedLimitComponent() {}
		SpeedLimitComponent(const ComponentProperties& properties);
	};

	/*
		Tags. These are basically components without any data. 
	*/
	struct TrackableComponent
	{
	};
}