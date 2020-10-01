#pragma once

#include "texture.h"
#include "tileset.h"
#include "renderer.h"
#include "../core/collisions.hpp"

#include "../core/typedefs.h"
#include "../util/vector2.h"

namespace hiage
{
	//class for the tilemap
	class __IMPORTEXPORT Tilemap
	{

	private:
		uint width;
		uint height;
		uint layers;
		uint tilesize;
		bool created;

		unsigned int ***tilemap;

		//texture list
		Tileset * tileset;

	public:
		Tilemap();
		Tilemap(int width, int height, int layers, int tilesize);
		~Tilemap();

		void createMap(int width, int height, int layers, int tilesize);
		void setTileset(Tileset * tileset);
		void destroy();

		void render(Renderer &renderer, float camx, float camy, float zoom, float aspect, ObjectZ depth, int layer);
		void importMap(unsigned int *** data);

		BoundingBox<double>	getTilesInRect(float left, float top, float right, float bottom) const;
		Vector2<double> getTileCoordinates(float x, float y, int layer) const;
		unsigned int 	getTile(uint x, uint y, uint z) const;
		void 			setTile(uint x, uint y, uint ulayer, uint tile);
		Tileset * 		getTileset() const { return tileset; }
		int 			getTileSize() const { return tilesize; }
		int 			getWidth();
		int 			getHeight();
		int				getLayers();
		bool			isLoaded() const;


	};
}
