#pragma once


#include "typedefs.h"

#include "../gfx/texture.h"
#include "../gfx/sprite.h"
#include "../gfx/tileset.h"
#include "../gfx/fonts.hpp"

#include "../util/exceptions.h"

#include "resourcedescriptors.hpp"

#include <vector>
#include <memory>
#include <filesystem>

namespace hiage
{
	template<typename T>
	class Resource
	{
	public:
		Resource<T>() {};

		//name of the resource
		std::string name;

		//the resource itself (texture, sprite etc.)
		T resource;

		//in case some information needs to be handled outside the resource manager
		int intData1, intData2, intData3;
		std::string strData1, strData2, strData3;
	};

	//template class to manage loaded resources (textures, sounds etc.)
	template <typename T>
	class ResourceManager
	{
	private:
		std::unordered_map<std::string, Resource<T>> resources;
		std::string dataRoot;

	protected:
		virtual std::string getResourceTypeName() const = 0;
		
		std::string getResourcePath(const std::string& relativePath) const
		{
			std::filesystem::path root = dataRoot;
			auto fullPath = root / relativePath;
			
			return fullPath.string();
		}

		// This function should load the data file with the resource (e.g. the JSON or XML file), and add the resource to the resources collection by calling initResource.
		virtual void loadResource(const std::string& file) = 0;

		Resource<T>& initResource(const std::string& name)
		{
			if (resources.find(name) != resources.end())
				throw Exception(std::string("ERROR: Resource already exist: ") + name + std::string(" of type ") + getResourceTypeName());

			resources[name] = Resource<T>();
			resources[name].name = name;
			return resources[name];
		}
		
	public:
		ResourceManager(const std::string& dataRoot) : dataRoot(dataRoot)
		{
		}
		virtual ~ResourceManager()
		{
		}

		//front-end for loading a resource.
		int load(const std::string& file, const std::string& name)
		{
			//check if the name already exists
			/*if (resources.find(name) != resources.end())
				return -1;*/
			
			// Load resource from file
			loadResource(file);

			// And get the reference
			auto& resource = resources[name];
			
			//if a name for the resource was passed to the function, override whatever name that was set from LoadResource.
			if (name.length() > 0)
			{
				resource.name = name;
			}

			return (int)(resources.size() - 1);
		}


		const Resource<T>& requestResourceRef(const std::string& name) const
		{
			if (resources.find(name) == resources.end())
				throw Exception(std::string("ERROR: Could not find resource ") + name + std::string(" of type ") + getResourceTypeName());

			return resources.at(name);
		}

		//returns a copy of a resource object
		Resource<T> requestResourceCopy(const std::string& name) const
		{
			return requestResourceRef(name);
		}
		
		int getResourceCount()
		{
		    return resources.size();
		}
	};



	//some specialized resource managers for the resources included in the engine
	class __IMPORTEXPORT TextureManager : public ResourceManager<Texture>
	{
	protected:
		virtual void loadResource(const std::string& path) override;
		virtual std::string getResourceTypeName() const override { return "Texture"; }
	public:
		TextureManager(const std::string& dataRoot) : ResourceManager(dataRoot) { }

		void loadTextureResource(const std::string& path, const std::string& name);
	};

	class __IMPORTEXPORT SpriteManager : public ResourceManager<Sprite>
	{
	protected:
		virtual void loadResource(const std::string& path) override;
		virtual std::string getResourceTypeName() const override { return "Sprite"; }
	public:
		SpriteManager(const std::string& dataRoot) : ResourceManager(dataRoot) { }
	};

	class __IMPORTEXPORT TilesetManager : public ResourceManager<Tileset>
	{
	private:
		TextureManager& _textureManager;
	protected:
		virtual void loadResource(const std::string& path) override;
		virtual std::string getResourceTypeName() const override { return "Tileset"; }
	public:
		TilesetManager(TextureManager& textureManager, const std::string& dataRoot) : ResourceManager(dataRoot), _textureManager(textureManager) { }
	};

	class __IMPORTEXPORT FontManager : public ResourceManager<Font>
	{
    protected:
        virtual void loadResource(const std::string& path) override;
		virtual std::string getResourceTypeName() const override { return "Font"; }
	public:
		FontManager(const std::string& dataRoot) : ResourceManager(dataRoot) { }
	};

	class __IMPORTEXPORT ObjectManager : public ResourceManager<ObjectDescriptor>
	{
	protected:
		virtual void loadResource(const std::string& path) override;
		virtual std::string getResourceTypeName() const override { return "Object"; }
	public:
		ObjectManager(const std::string& dataRoot) : ResourceManager(dataRoot) { }
	};
}
