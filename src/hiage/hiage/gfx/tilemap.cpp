#include "tilemap.hpp"
#include "../util/exceptions.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

#include <GL/gl.h>
#include <cmath>

using namespace hiage;
using namespace std;

Tilemap::~Tilemap()
{
	destroy();
}

Tilemap::Tilemap() : _created(false), _tilesize(32)
{
	_tilemap.clear();
	_tileset = 0;
	_width = 0;
	_height = 0;
	_layers = 0;
}

Tilemap::Tilemap(int width, int height, int layers, int tilesize) : _created(false)
{
	createMap(width,height,layers,tilesize);

	_created = true;

	_tileset = 0;
}

void Tilemap::createMap(int width, int height, int layers, int tilesize)
{
	if (_created)
	{
		destroy();
	}

	this->_width = width;
	this->_height = height;
	this->_layers = layers;
	this->_tilesize = tilesize;
    
	cout << "CREATING MAP WITH " << width << " " << height << " " << layers << endl;
	clog << "Allocating memory for tile map...\n" << flush;

	//allocate memory for the tilemap
	_tilemap.resize((size_t)width * height * layers, 0);

	clog << "OK: Tilemap allocated successfully.\n" << flush;
	_created = true;
}

//import a map
void Tilemap::importMap(const std::vector<uint32_t>& data)
{
	if (data.size() != _tilemap.size())
	{
		throw runtime_error("Tilemap::import: Mismatched sizes!");
	}
	_tilemap = data;
}


BoundingBox<double> Tilemap::getTilesInRect(double left, double top, double right, double bottom) const
{
	//check that the rect is inside the tilemap
	float tilemap_right, tilemap_left, tilemap_top, tilemap_bottom;
	BoundingBox<double> rect(0,0,0,0);

	tilemap_left = 0;
	tilemap_right = (float)(_tilesize * _width);
	tilemap_top = (float)(_tilesize * _height);
	tilemap_bottom = 0;

	if ((right > tilemap_left) && (left < tilemap_right) && (top > tilemap_bottom) && (bottom < tilemap_top))
	{
		if (left < tilemap_left)
		{
			left = tilemap_left;
		}
		if (top > tilemap_top)
		{
			top = tilemap_top;
		}
		if (bottom < tilemap_bottom)
		{
			bottom = tilemap_bottom;
		}
		if (right > tilemap_right)
		{
			right = tilemap_right;
		}

		left /= _tilesize;
		right /= _tilesize;
		top /= _tilesize;
		bottom /= _tilesize;

		rect.left = (int)floor(left);
		rect.right = (int)ceil(right);
		rect.top = (int)ceil(top);
		rect.bottom = (int)floor(bottom);
	}

	return rect;
}

Vector2<double> Tilemap::getTileCoordinates(double x, double y, int layer) const
{
	if (x >= 0 && y >= 0 && layer >= 0)
	{
		return Vector2<double>(x / _tilesize, y / _tilesize);
	}

	return Vector2<double>(-1,-1);
}

unsigned int Tilemap::getTile(uint x, uint y, uint z) const
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height) || (_created == false))
	{
		return 0;
	}

	return _tilemap[(size_t)z * _height * _width + (size_t)y * _width + (size_t)x];
}

const std::vector<uint32_t>& hiage::Tilemap::getTilemapRaw()
{
	return _tilemap;
}

void Tilemap::destroy()
{
    cout << "DESTROYING MAP WITH " << _width << " " << _height << " " << _layers << endl;
	
	_width = 0;
	_height = 0;
	_layers = 0;

	_tilemap.clear();
}

void Tilemap::render(Renderer &renderer, double camx, double camy, double zoom, double aspect, ObjectZ depth, int layer)
{
	uint x,y;

	if (!_tileset)
	{
//		clog << "Warning: Cannot render tilemap: No tile set is set.\n" << flush;
		return;
	}

	if (_tileset->getTileCount() == 0)
	{
	//	clog << "Warning: Cannot render tilemap: At least 1 texture must be in the tile set.\n" << flush;;
		return;
	}

	double xmin;
	double ymin;
	double xmax;
	double ymax;

	//calculate the first and last tiles to be rendered (optimization) so the render loop does not go through EVERY tile
	//if the map is 500x500, the loop must loop 250000 times with one layer without optimization, but with optimization,
	//it may have to loop just 100 times, more or less, depending on how close the cam is zoomed in

	if (aspect > 1.0f)
	{
		xmin = (camx - (zoom * aspect)) / _tilesize;
		ymin = (camy - zoom) / _tilesize;// * aspect;
		xmax = xmin + (((zoom * 2) * aspect) / _tilesize) + 1;
		ymax = ymin + ((zoom * 2) / _tilesize) + 1;
	}
	else
	{
		xmin = (camx - zoom) / _tilesize;
		ymin = (camy - (zoom / aspect)) / _tilesize;
		xmax = xmin + ((zoom * 2) / _tilesize) + 1;
		ymax = ymin + (((zoom * 2) / aspect) / _tilesize) + 1;
	}

	xmin -= 1.0f;
	xmax += 1.0f;

	ymin -= 1.0f;
	ymax += 1.0f;

	if (xmin < 0.0f) xmin = 0.0f;
	if (xmin > (double)_width) xmin = (double)_width;

	if (ymin < 0.0f) ymin = 0.0f;
	if (ymin > (double)_height) ymin = (double)_height;

	if (xmax < 0.0f) xmax = 0.0f;
	if (xmax > (double)_width) xmax = (double)_width;

	if (ymax < 0.0f) ymax = 0.0f;
	if (ymax > (double)_height) ymax = (double)_height;

	for (x = (uint)xmin; x < (uint)xmax; x++)
	{
		for (y = (uint)ymin; y < (uint)ymax; y++)
		{
		    if (_tilemap[(size_t)layer * _height * _width + (size_t)y * _width + x] == 0)
                continue;
            renderer.beginRender(depth, _tileset->getTile(_tilemap[(size_t)layer * _height * _width + (size_t)y * _width + x]).texture);
			renderer.addVertex((double)x * _tilesize, (double)y * _tilesize + _tilesize, 0, 0);
			renderer.addVertex((double)x * _tilesize + _tilesize, (double)y * _tilesize + _tilesize, 1, 0);
			renderer.addVertex((double)x * _tilesize + _tilesize, (double)y * _tilesize, 1, 1);
			renderer.addVertex((double)x * _tilesize, (double)y * _tilesize, 0, 1);
            renderer.endRender();
		}
	}


//	tileset->updateTiles();
}

void Tilemap::setTileset(Tileset * tileset)
{
	if (!tileset)
	{
		throw Exception("ERROR: No tileset specified.");
	}
	this->_tileset = tileset;
}

void Tilemap::setTile(uint x, uint y, uint z, uint tile)
{
	if (!_tileset)
	{
		return;
	}

	if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height) && (z >= 0) && (z < _layers))
	{
		//if ((tile >= 0) && (tile < tileset->getTileCount()))
		if (tile >= 0)
		{
			_tilemap[z * _height * _width + y * _width + x] = tile;
		}
	}
}

int Tilemap::getWidth()
{
	return _width;
}

int Tilemap::getHeight()
{
	return _height;
}

int Tilemap::getLayers()
{
	return _layers;
}

bool hiage::Tilemap::isLoaded() const
{
	return _created;
}
