#pragma once

#include "texture.h"
#include "tileset.h"
#include "renderer.h"
#include "../core/collisions.hpp"

#include "../core/typedefs.h"
#include "../util/vector2.h"

#include <cstdint>

namespace hiage
{
	//class for the tilemap
	class __IMPORTEXPORT Tilemap
	{

	private:
		uint _width;
		uint _height;
		uint _layers;
		uint _tilesize;
		bool _created;

		// Tiles stored in row major order
		std::vector<uint32_t> _tilemap;

		//texture list
		Tileset * _tileset;

	public:
		Tilemap();
		Tilemap(int width, int height, int layers, int tilesize);
		Tilemap(const Tilemap&) = delete;
		~Tilemap();

		void createMap(int width, int height, int layers, int tilesize);
		void setTileset(Tileset * tileset);
		void destroy();

		void render(Renderer &renderer, double camx, double camy, double zoom, double aspect, ObjectZ depth, int layer);
		void importMap(const std::vector<uint32_t>& data);

		BoundingBox<double>	getTilesInRect(double left, double top, double right, double bottom) const;
		Vector2<double> getTileCoordinates(double x, double y, int layer) const;
		unsigned int 	getTile(uint x, uint y, uint z) const;
		const std::vector<uint32_t>& getTilemapRaw();
		void 			setTile(uint x, uint y, uint ulayer, uint tile);
		Tileset * 		getTileset() const { return _tileset; }
		int 			getTileSize() const { return _tilesize; }
		int 			getWidth();
		int 			getHeight();
		int				getLayers();
		bool			isLoaded() const;
	};
}