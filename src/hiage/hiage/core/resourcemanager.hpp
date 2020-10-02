#pragma once


#include "typedefs.h"

#include "../gfx/texture.h"
#include "../gfx/sprite.h"
#include "../gfx/tileset.h"
#include "../gfx/fonts.h"

#include "../util/exceptions.h"

#include <vector>
#include <memory>

namespace hiage
{
	template<typename T>
	class Resource
	{
	public:
		//name of the resource
		std::string name;

		//the resource itself (texture, sprite etc.)
		T* resource;

		//in case some information needs to be handled outside the resource manager
		int intData1, intData2, intData3;
		std::string strData1, strData2, strData3;
	};

	//template class to manage loaded resources (textures, sounds etc.)
	template <typename T>
	class ResourceManager
	{
	private:
		std::vector<std::unique_ptr<Resource<T>>> resources;

	private:
		int findResourceIndex(const std::string& name) const
		{
			if (name.length() == 0)
			{
				return -1;
			}
			for (unsigned int i = 0; i < resources.size(); i++)
			{
				if (!resources[i]->name.compare(name))
				{
					return i;
				}
			}

			return -1;
		}

	protected:
		virtual std::unique_ptr<Resource<T>> loadResource(const std::string& file) = 0;

	public:
		virtual ~ResourceManager()
		{
		}

		//front-end for loading a resource.
		int load(const std::string& file, const std::string& name)
		{
			//check if the name already exists
			if (findResourceIndex(name) >= 0)
			{
				return -1;
			}

			auto resource = loadResource(file);

			if (resource == nullptr)
			{
				return -1;
			}
			
			//if a name for the resource was passed to the function, override whatever name that was set from LoadResource.
			if (name.length() > 0)
			{
				resource->name = name;
			}

			resources.push_back(std::move(resource));
			
			return (int)(resources.size() - 1);
		}


		const std::unique_ptr<Resource<T>>& requestResourcePtr(const std::string& name) const
		{
			int index = findResourceIndex(name);
			if (index >= 0)
			{
				return resources[index];
			}

			throw Exception(string("ERROR: Could not find resource ") + name);
		}

		const std::unique_ptr<Resource<T>>& requestResourcePtr(unsigned int index) const
		{
			if (index < resources.size() && index >= 0)
			{
				return resources[index];
			}

			throw Exception(string("ERROR: Could not find resource with index ") + index);
		}

		//returns a copy of a resource object
		std::unique_ptr<Resource<T>> requestResourceCopy(std::string name)
		{
			int index = findResourceIndex(name);
			if (index >= 0)
			{
				Resource<T> * copy = new Resource<T>;
				*copy = *resources[index];
				copy->resource = new T;
				*copy->resource = *resources[index]->resource;

				return std::unique_ptr<Resource<T>>(copy);
			}

			throw Exception(string("ERROR: Could not find resource ") + name);
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
		virtual std::unique_ptr<Resource<Texture>> loadResource(const std::string& path) override;
	};

	class __IMPORTEXPORT SpriteManager : public ResourceManager<Sprite>
	{
	protected:
		virtual std::unique_ptr<Resource<Sprite>> loadResource(const std::string& path) override;
	};

	class __IMPORTEXPORT TilesetManager : public ResourceManager<Tileset>
	{
	protected:
		virtual std::unique_ptr<Resource<Tileset>> loadResource(const std::string& path) override;
	};

	class __IMPORTEXPORT FontManager : public ResourceManager<Font>
	{
    protected:
        virtual std::unique_ptr<Resource<Font>> loadResource(const std::string& path) override;
	};
}
