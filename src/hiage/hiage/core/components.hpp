#pragma once

#include "../util/vector2.h"
#include "../gfx/sprite.h"
#include "collisions.hpp"

#include <unordered_map>
#include <memory>
#include <variant>

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


	class PhysicsComponent : public DatalessComponent<4>
	{
		using DatalessComponent::DatalessComponent;
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

	/*
	ComponentManager
	*/

	class ComponentManager
	{
	private: 
		// TODO - Make it possible to register new component factory classes
		std::unique_ptr<Component> createRenderable(const std::unordered_map<std::string, std::variant<std::string, double>>& properties) const;

		Game& game;
	public:
		ComponentManager(Game& game);
		~ComponentManager();
		std::unique_ptr<Component> createComponent(const std::string& type, const std::unordered_map<std::string, std::variant<std::string, double>>& properties, const std::map<std::string, void*>& runtimeAttributes) const;
	};
}