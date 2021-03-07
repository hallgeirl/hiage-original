#pragma once

#include "../util/vector2.h"
#include "../gfx/sprite.h"
#include "collisions.hpp"

#include <flecs.h>

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <variant>
#include "resourcedescriptors.hpp"

namespace hiage 
{
	/*
	Components
	*/

	class Component
	{
	protected:
		Component() { }
	public:
		virtual ~Component() { }
	};

	/*
	* A simple component containing a set of data. 
	*/
	template<typename T>
	class GenericComponent : public Component
	{
	private:
		T data;
	public:
		GenericComponent() : Component(), data() { }
		GenericComponent(const GenericComponent<T>& c) : Component() { *this = c; }
		GenericComponent(const T& data) : Component(), data(data) { }
		
		T& getData() { return data; }
		void setData(const T& newValue) { data = newValue; }

		// Override this to define how the component's state is created from properties.
		virtual T createState(const ComponentProperties&) 
		{ 
			return T();
		}
	};

	/*
	* A templated component that can be used for defining components that doesn't own any data, i.e. "tag" type components.
	*/
	class DatalessComponent : public GenericComponent<int>
	{
	public:
		DatalessComponent() : GenericComponent<int>() { }
		DatalessComponent(const DatalessComponent& c) : GenericComponent() { *this = c; }
	};

	/*
	* Concrete component definitions
	*/
	struct PositionComponent
	{
		Vec2d pos;

		PositionComponent() : pos(0, 0) { }
		PositionComponent(const ComponentProperties& properties);
	};

	class VelocityComponent : public GenericComponent<Vector2<double>>
	{
		using GenericComponent::GenericComponent;
	};

	struct RenderableComponent
	{
		Sprite sprite;
	};


	struct PhysicsProperties
	{
		double airResistance, groundFriction;
		
		bool hasGravity;
	};

	class PhysicsComponent : public GenericComponent<PhysicsProperties>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual PhysicsProperties createState(const ComponentProperties& properties) override;
	};

	struct ControllerProperties
	{
		std::string controllerType;
		std::vector<std::string> constantActions;

	};
	class ControllerComponent : public GenericComponent<ControllerProperties>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual ControllerProperties createState(const ComponentProperties& properties) override;
	};

	/*
	Collision events
	*/
	struct ObjectTileCollisionData
	{
		int entityId;
		CollisionResult collisionResult;
	};
	
	struct ObjectObjectCollisionData
	{
		int entityId1, entityId2;
		CollisionResult collisionResult;
	};

	struct CollidableProperties
	{
		BoundingPolygon boundingPolygon;
		std::vector<ObjectObjectCollisionData> objectCollisions;
		std::vector<ObjectTileCollisionData> tileCollisions;
	};

	class CollidableComponent : public GenericComponent<CollidableProperties>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual CollidableProperties createState(const ComponentProperties& properties) override;
	};

	struct TrackingComponentProperties
	{
		std::string mode;
	};
	class TrackingComponent : public GenericComponent<TrackingComponentProperties>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual TrackingComponentProperties createState(const ComponentProperties& properties) override;
	};

	class TrackableComponent : public DatalessComponent
	{
		using DatalessComponent::DatalessComponent;
	};

	struct CameraProperties
	{
		double zoom;
	};
	class CameraComponent : public GenericComponent<CameraProperties>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual CameraProperties createState(const ComponentProperties& properties) override;
	};

	// Used for "object state" when it comes to animations (e.g. "on ground", "standing", "walking", "jumping", "falling") and allowed actions (e.g. jumping is allowed when standing, etc.)
	// The actual rules are defined by the system that handles this component. This is just the container of the state name and metadata attached to the state.
	struct State
	{
		std::string stateName;
		std::unordered_map<std::string, std::variant<std::string, int, double>> metadata;
	};
	class StateComponent : public GenericComponent<State>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual State createState(const ComponentProperties& properties) override;
	};

	class ControllerStateComponent : public GenericComponent<std::unordered_set<std::string>>
	{
	public:
		using GenericComponent::GenericComponent;
	};

	struct SpeedLimitState
	{
		Vec2d speedLimit;
	};
	class SpeedLimitComponent : public GenericComponent<SpeedLimitState>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual SpeedLimitState createState(const ComponentProperties& properties) override;
	};
}