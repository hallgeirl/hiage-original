#pragma once

#include "components.hpp"
#include "resourcedescriptors.hpp"

#include <flecs.h>
#include <string>
#include <memory>

namespace hiage 
{
    class Game;
	/*
	    ComponentFactory + specialized factories
	*/
	class ComponentFactory
	{
	public:
		virtual ~ComponentFactory() {};
		virtual const flecs::entity& createComponent(flecs::entity& entity, const ComponentProperties& properties) const = 0;
	};

	template<typename T>
	class GenericComponentFactory : public ComponentFactory
	{
	public:
		/// <summary>
		/// Creates the new component from properties only. Useful for adding components to an object that isn't part of the object definition.
		/// </summary>
		/// <param name="properties"></param>
		/// <returns></returns>
		virtual const flecs::entity& createComponent(flecs::entity& entity, const ComponentProperties& properties) const override
		{
			return entity.set<T>({ properties });
		}
	};

	template<typename T>
	class TagComponentFactory : public ComponentFactory
	{
	public:
		/// <summary>
		/// Creates the new component from properties only. Useful for adding components to an object that isn't part of the object definition.
		/// </summary>
		/// <param name="properties"></param>
		/// <returns></returns>
		virtual const flecs::entity& createComponent(flecs::entity& entity, const ComponentProperties& properties) const override
		{
			return entity.add<T>();
		}
	};

	class RenderableComponentFactory : public ComponentFactory
	{
	private:
		const Game& game;
	public:
		RenderableComponentFactory(const Game& game);
		virtual const flecs::entity& createComponent(flecs::entity& entity, const ComponentProperties& runtimeProperties) const override;
	};


    /*
        ComponentManager (it's really ComponentFactoryManager though)
    */
	class ComponentManager
	{
	private: 
		std::unordered_map<std::string, std::unique_ptr<ComponentFactory>> _componentFactories;
		Game& game;
	public:
		ComponentManager(Game& game);
		~ComponentManager();

		template<typename T, typename TComponent, typename ...TRest>
		void addComponentFactory(const std::string& componentType, TRest... args)
		{
			_componentFactories[componentType] = std::make_unique<T>(args...);
		}

		template<typename TComponent, typename ...TRest>
		void addGenericComponentFactory(const std::string& componentType, TRest... args)
		{
			_componentFactories[componentType] = std::make_unique<GenericComponentFactory<TComponent>>(args...);
		}

		template<typename TComponent, typename ...TRest>
		void addTagComponentFactory(const std::string& componentType, TRest... args)
		{
			_componentFactories[componentType] = std::make_unique<TagComponentFactory<TComponent>>(args...);
		}


		const flecs::entity& createComponent(flecs::entity& entity, const ComponentDescriptor& componentDescriptor, const ComponentProperties& runtimeProperties) const;
		const flecs::entity& createComponent(flecs::entity& entity, const std::string& type, const ComponentProperties& properties) const;
	};
}