#pragma once


#include <vector>
#include <string>

#include "gfx_texture.h"
#include "com_typedefs.h"

namespace Hinage
{
	class __IMPORTEXPORT Tileset
	{
	public:
		struct  Tile
		{
		    int id;
			int block;
			Texture * texture;
			int delay, nextTile, counter;
		};

        class iterator
        {
        private:
            Tile *data;
            bool initialized;

        public:
            iterator()
            {
                initialized = false;
                data = 0;
            }

            iterator(const iterator & iter)
            {
                data = iter.data;
                initialized = true;
            }

            iterator(Tile * data)
            {
                this->data = data;
            }

            //prefix ++
            iterator operator++()
            {
                iterator old = *this;
                if (initialized)
                {
                    while ((++data)->id == -1 && data->id != -2);
                }

                return old;
            }

            //postfix ++
            iterator operator++(int)
            {
                if (initialized)
                {
                    while ((++data)->id == -1 && data->id != -2);
                }

                return *this;
            }

            //dereferencing operator
            Tile &operator*()
            {
                return *data;
            }

            //assignment
            iterator operator=(const iterator & iter)
            {
                data = iter.data;

                return *this;
            }

            bool operator==(const iterator & iter)
            {
                return (data->id == iter.data->id);
            }

            bool operator!=(const iterator & iter)
            {
                return (data->id != iter.data->id);
            }

            iterator operator-(int i)
            {
                iterator iter(*this);
                iter.data -= i;
                return iter;
            }

            iterator operator+(int i)
            {
                iterator iter(*this);
                iter.data += i;
                return iter;
            }
        };

	private:
		static const int MAX_TILES = 4096;

		//the tiles that are rendered
		Tile tiles[MAX_TILES];
		//stores the original tiles in the tileset (those will not change because of the animations)
		Tile tilesOrg[MAX_TILES];

		int tileCount;
		int last, first; //for optimization

        std::string textureFiles[MAX_TILES];  //store the texture filenames (might come in handy)

	public:
		Tileset();
		~Tileset();

		iterator begin();
		iterator end();

		bool addTile(Texture * texture, int block, int nextTile, int delay, std::string textureFileName, int id = -1);

		void selectTile(int tileID) { tiles[tileID].texture->select(); }
		uint getTileCount() const { return tileCount; }
		std::string getTextureFileName(int tileID);

		//void updateTiles();
		Tile getTile(int index) const;

	};
}
