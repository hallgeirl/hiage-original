#pragma once


#include "resmanager.h"
#include "typedefs.h"

#include "../gfx/texture.h"
#include "../gfx/sprite.h"
#include "../gfx/tileset.h"
#include "../gfx/fonts.h"

#include "../util/exceptions.h"

#include <vector>

namespace hiage
{
	//template class to manage loaded resources (textures, sounds etc.)
	template <class T> class ResourceManager
	{
	public:
		class Resource
		{
		public:
			//name of the resource
			std::string name;

			//the resource itself (texture, sprite etc.)
			T * resource;

			//in case some information needs to be handled outside the resource manager
			int intData1, intData2, intData3;
			std::string strData1, strData2, strData3;
		};

	private:
		std::vector<Resource *>	resources;

	private:
		int findResourceIndex(std::string name)
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
		virtual Resource * loadResource(const std::string& file) = 0;

	public:
		virtual ~ResourceManager()
		{
			for (unsigned int i = 0; i < resources.size(); i++)
			{
				delete resources[i];
			}
		}

		//front-end for loading a resource.
		int load(const std::string& file, const std::string& name)
		{
			//check if the name already exists
			if (findResourceIndex(name) >= 0)
			{
				return -1;
			}

			Resource * resource = loadResource(file);

			if (!resource)
			{
				return -1;
			}

			//if a name for the resource was passed to the function, override whatever name that was set from LoadResource.
			if (name.length() > 0)
			{
				resource->name = name;
			}

			resources.push_back(resource);

			return (int)(resources.size() - 1);
		}


		Resource * requestResourcePtr(const char * name)
		{
			int index = findResourceIndex(name);
			if (index >= 0)
			{
				return resources[index];
			}

			throw Exception(string("ERROR: Could not find resource ") + name);
		}

		Resource * requestResourcePtr(unsigned int index)
		{
			if (index < resources.size() && index >= 0)
			{
				return resources[index];
			}

			throw Exception(string("ERROR: Could not find resource with index ") + index);
		}
		// TODO - Fix this so we use smart pointers.
		//returns a copy of a resource object
		Resource * requestResourceCopy(std::string name)
		{
			int index = findResourceIndex(name);
			if (index >= 0)
			{
				Resource * copy = new Resource;
				*copy = *resources[index];
				copy->resource = new T;
				*copy->resource = *resources[index]->resource;

				return copy;
			}

			throw Exception(string("ERROR: Could not find resource ") + name);
		}

		/*
		Resource * requestResourceCopy(int index)
		{
			if (index < resources.size())
			{
				Resource * copy = new Resource;

				*copy = *resources[index];

				return copy;
			}

			throw Exception(string("ERROR: Could not find resource with index ") + index);
		}
		*/
		int getResourceCount()
		{
		    return resources.size();
		}
	};


	//some specialized resource managers for the resources included in the engine
	class __IMPORTEXPORT TextureManager : public ResourceManager<Texture>
	{
	protected:
		virtual Resource * loadResource(const std::string& path) override;
	};

	class __IMPORTEXPORT SpriteManager : public ResourceManager<Sprite>
	{
	protected:
		virtual Resource * loadResource(const std::string& path) override;
	};

	class __IMPORTEXPORT TilesetManager : public ResourceManager<Tileset>
	{
	protected:
		virtual Resource * loadResource(const std::string& path) override;
	};

	class __IMPORTEXPORT FontManager : public ResourceManager<Font>
	{
    protected:
        virtual Resource * loadResource(const std::string& path) override;
	};
}