#pragma once

#include "../util/vector2.h"
#include "../gfx/sprite.h"
#include "collisions.hpp"

#include <unordered_map>
#include <unordered_set>
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
	* A simple component containing a set of data. 
	*/
	template<typename T, int TypeID>
	class GenericComponent : public Component
	{
	private:
		T data;
	public:
		GenericComponent() : Component(TypeID), data() { }
		GenericComponent(const GenericComponent<T, TypeID>& c) : Component(TypeID) { *this = c; }
		GenericComponent(const T& data) : Component(TypeID), data(data) { }
		
		T& getData() { return data; }
		void setData(const T& newValue) { data = newValue; }

		// Override this to define how the component's state is created from properties.
		virtual T createState(const ComponentProperties&) 
		{ 
			return T();
		}

		static const int TYPEID = TypeID;
	};

	/*
	* A templated component that can be used for defining components that doesn't own any data, i.e. "tag" type components.
	*/
	template<int TypeID>
	class DatalessComponent : public GenericComponent<int, TypeID>
	{
	public:
		DatalessComponent() : GenericComponent() { }
		DatalessComponent(const DatalessComponent<TypeID>& c) : GenericComponent() { *this = c; }
	};

	/*
	* Concrete component definitions
	*/
	class PositionComponent : public GenericComponent<Vector2<double>, 1>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual Vector2<double> createState(const ComponentProperties& properties) override;
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
	public:
		using GenericComponent::GenericComponent;
		virtual PhysicsProperties createState(const ComponentProperties& properties) override;
	};

	struct ControllerProperties
	{
		std::string controllerType;
		std::vector<std::string> constantActions;

	};
	class ControllerComponent : public GenericComponent<ControllerProperties, 5>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual ControllerProperties createState(const ComponentProperties& properties) override;
	};

	class CollidableComponent : public GenericComponent<BoundingPolygon, 7>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual BoundingPolygon createState(const ComponentProperties& properties) override;
	};

	struct TrackingComponentProperties
	{
		std::string mode;
	};
	class TrackingComponent : public GenericComponent<TrackingComponentProperties, 8>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual TrackingComponentProperties createState(const ComponentProperties& properties) override;
	};

	class TrackableComponent : public DatalessComponent<9>
	{
		using DatalessComponent::DatalessComponent;
	};

	struct CameraProperties
	{
		double zoom;
	};
	class CameraComponent : public GenericComponent<CameraProperties, 10>
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
	class StateComponent : public GenericComponent<State, 11>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual State createState(const ComponentProperties& properties) override;
	};

	class ControllerStateComponent : public GenericComponent<std::unordered_set<std::string>, 12>
	{
	public:
		using GenericComponent::GenericComponent;
	};

	struct SpeedLimitState
	{
		Vec2d speedLimit;
	};
	class SpeedLimitComponent : public GenericComponent<SpeedLimitState, 13>
	{
	public:
		using GenericComponent::GenericComponent;
		virtual SpeedLimitState createState(const ComponentProperties& properties) override;
	};
	/*
	ComponentManager
	*/

	class ComponentFactory
	{
	public:
		virtual ~ComponentFactory() {};
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties& runtimeProperties) const = 0;
		virtual std::unique_ptr<Component> createComponent(const ComponentProperties& properties) const = 0;
	};

	template<typename T>
	class GenericComponentFactory : public ComponentFactory
	{
	public:
		/// <summary>
		/// Creates the component using a component descriptor (basically a "blueprint" from the object definition), 
		/// as well as runtime properties, that are typically set at runtime when creating the object, like position etc.
		/// </summary>
		/// <param name="componentDescriptor"></param>
		/// <param name="runtimeProperties"></param>
		/// <returns></returns>
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties& runtimeProperties) const override
		{
			ComponentProperties mergedProperties;
			mergedProperties.insert(runtimeProperties.begin(), runtimeProperties.end());
			mergedProperties.insert(componentDescriptor.properties.begin(), componentDescriptor.properties.end());
			auto component = std::make_unique<T>();
			auto state = component->createState(mergedProperties);
			component->setData(state);

			return std::move(component);
		}

		/// <summary>
		/// Creates the new component from properties only. Useful for adding components to an object that isn't part of the object definition.
		/// </summary>
		/// <param name="properties"></param>
		/// <returns></returns>
		virtual std::unique_ptr<Component> createComponent(const ComponentProperties& properties) const override
		{
			auto component = std::make_unique<T>();
			auto state = component->createState(properties);
			component->setData(state);

			return std::move(component);
		}
	};

	class RenderableComponentFactory : public ComponentFactory
	{
	private:
		const Game& game;
	public:
		RenderableComponentFactory(const Game& game);
		virtual std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties& runtimeProperties) const override;
		virtual std::unique_ptr<Component> createComponent(const ComponentProperties& runtimeProperties) const override;
	};

	class ComponentManager
	{
	private: 
		std::unordered_map<std::string, std::unique_ptr<ComponentFactory>> _componentFactories;
		std::unordered_map<std::string, int> _componentNameToTypeId;
		Game& game;
	public:
		ComponentManager(Game& game);
		~ComponentManager();

		template<typename T, typename TComponent, typename ...TRest>
		void addComponentFactory(const std::string& componentType, TRest... args)
		{
			_componentFactories[componentType] = std::make_unique<T>(args...);
			_componentNameToTypeId[componentType] = TComponent::TYPEID;
		}

		template<typename TComponent, typename ...TRest>
		void addGenericComponentFactory(const std::string& componentType, TRest... args)
		{
			_componentFactories[componentType] = std::make_unique<GenericComponentFactory<TComponent>>(args...);
			_componentNameToTypeId[componentType] = TComponent::TYPEID;
		}

		std::unique_ptr<Component> createComponent(const ComponentDescriptor& componentDescriptor, const ComponentProperties& runtimeProperties) const;
		std::unique_ptr<Component> ComponentManager::createComponent(const std::string& type, const ComponentProperties& properties) const;
		int getTypeIdForComponentType(const std::string& componentType) const;
	};
}