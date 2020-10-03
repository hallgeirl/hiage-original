#pragma once

#include "../util/vector2.h"
#include "../gfx/sprite.h"
#include "collisions.hpp"

#include <unordered_map>
#include <memory>
#include <variant>
#include "resourcedescriptors.hpp"

namespace hiage 
{
	class Game;

	/*
	Components
	*/

	class Component
	{
	private:
		int typeId; // Component type
	protected:
		Component(int typeId) : typeId(typeId) { }
	public:
		virtual ~Component() { }
		int getTypeId() { return typeId; }
	};

	/*
	* A templated component that can be used for defining components that doesn't own any data, i.e. "tag" type components.
	*/
	template<int TypeID>
	class DatalessComponent : public Component
	{
	public:
		DatalessComponent() : Component(TypeID) { }
		DatalessComponent(const DatalessComponent<TypeID>& c) : Component(TypeID) { *this = c; }
		
		static const int TYPEID = TypeID;
	};

	/*
	* A simple component containing a set of data. 
	*/
	template<typename T, int TypeID>
	class GenericComponent : public DatalessComponent<TypeID>
	{
	private:
		T data;
	public:
		GenericComponent() : DatalessComponent() { }
		GenericComponent(const GenericComponent<T, TypeID>& c) : DatalessComponent() { *this = c; }
		GenericComponent(const T& data) : DatalessComponent(), data(data) { }

		T& getData() { return data; }
		void setData(const T& newValue) { data = newValue; }
	};

	/*
	* Concrete component definitions
	*/
	class PositionComponent : public GenericComponent<Vector2<double>, 1>
	{
		using GenericComponent::GenericComponent;
	};

	class VelocityComponent : public GenericComponent<Vector2<double>, 2>
	{
		using GenericComponent::GenericComponent;
	};

	class RenderableComponent : public GenericComponent<Sprite, 3>
	{
		using GenericComponent::GenericComponent;
	};


	struct PhysicsProperties
	{
		double airResistance, groundFriction;
		
		bool hasGravity;
	};

	class PhysicsComponent : public GenericComponent<PhysicsProperties, 4>
	{
		using GenericComponent::GenericComponent;
	};

	class HumanControllerComponent : public DatalessComponent<5>
	{
		using DatalessComponent::DatalessComponent;
	};

	class BoundingBoxComponent : public GenericComponent<BoundingPolygon, 6>
	{
		using GenericComponent::GenericComponent;
	};

	class CollidableComponent : public DatalessComponent<7>
	{
		using DatalessComponent::DatalessComponent;
	};

	// Used for "object state" when it comes to animations (e.g. "on ground", "standing", "walking", "jumping", "falling") and allowed actions (e.g. jumping is allowed when standing, etc.)
	// The actual rules are defined by the system that handles this component. This is just the container of the state name and metadata attached to the state.

	struct State
	{
		std::string stateName;
		std::unordered_map < std::string, std::variant<std::string, int, double>> metadata;
	};

	class StateComponent : public GenericComponent<State, 8>
	{
		using GenericComponent::GenericComponent;
	};

	/*
	ComponentManager
	*/

	class ComponentFactory
	{
	public:
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const = 0;
	};

	template<typename T>
	class GenericComponentFactory : public ComponentFactory
	{
	public:
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const override
		{
			return std::make_unique<T>();
		}

	};

	class PhysicsComponentFactory : public ComponentFactory
	{
	public:
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const override;
	};

	class BoundingBoxComponentFactory : public ComponentFactory
	{
	public:
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const override;
	};

	class PhysicalComponentFactory : public ComponentFactory
	{
	public:
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const override;
	};

	class RenderableComponentFactory : public ComponentFactory
	{
	private:
		const Game& game;
	public:
		RenderableComponentFactory(const Game& game);
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const override;
	};

	class StateComponentFactory : public ComponentFactory
	{
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const override;
	};

	class ComponentManager
	{
	private: 
		// TODO - Make it possible to register new component factory classes
		std::unique_ptr<Component> createRenderable(const std::unordered_map<std::string, std::variant<std::string, double>>& properties) const;
		
		std::unordered_map<std::string, std::unique_ptr<ComponentFactory>> componentFactories;
		Game& game;
	public:
		ComponentManager(Game& game);
		~ComponentManager();

		template<typename T, typename ...TRest>
		void addComponentFactory(const std::string& componentType, TRest... args)
		{
			componentFactories[componentType] = std::make_unique<T>(args...);
		}

		std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const std::unordered_map<std::string, std::variant<std::string, double>>& runtimeProperties) const;
	};
}