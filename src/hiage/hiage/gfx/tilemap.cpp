#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

#include <GL/gl.h>
#include <math.h>
#include "tilemap.h"
#include "../util/exceptions.h"


using namespace hiage;
using namespace std;

Tilemap::~Tilemap()
{
	destroy();
}

Tilemap::Tilemap() : created(false), tilesize(32)
{
	tilemap = 0;
	tileset = 0;
	width = 0;
	height = 0;
	layers = 0;
}

Tilemap::Tilemap(int width, int height, int layers, int tilesize)
{
	createMap(width,height,layers,tilesize);

	created = true;

	tileset = 0;
}

void Tilemap::createMap(int width, int height, int layers, int tilesize)
{
	if (tilemap)
	{
		destroy();
	}

	this->width = width;
	this->height = height;
	this->layers = layers;
	this->tilesize = tilesize;
	int x,y,z;

    cout << "CREATING MAP WITH " << width << " " << height << " " << layers << endl;
	clog << "Allocating memory for tile map...\n" << flush;

	//allocate memory for the tilemap
	if ((tilemap = new unsigned int**[width]) == NULL)
	{
		throw Exception("ERROR: Could not allocate memory for tile map.");
	}

	for (x = 0; x < width; x++)
	{
		if ((tilemap[x] = new unsigned int*[height]) == NULL)
		{
			throw Exception("ERROR: Could not allocate memory for tile map.");
		}
	}

	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
			if ((tilemap[x][y] = new unsigned int[layers]) == NULL)
			{
				throw Exception("ERROR: Could not allocate memory for tile map.");
			}
		}
	}

	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
			for (z = 0; z < layers; z++)
			{
				tilemap[x][y][z] = 0;
			}
		}
	}

	clog << "OK: Tilemap allocated successfully.\n" << flush;
	created = true;
}

//import a map
void Tilemap::importMap(unsigned int *** data)
{
	uint x,y,z;

	clog << "Importing map...\n" << flush;

	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
			for (z = 0; z < layers; z++)
			{
				tilemap[x][y][z] = data[x][y][z];
			}
		}
	}
}

Rect Tilemap::getTilesInRect(float left, float top, float right, float bottom) const
{
	//check that the rect is inside the tilemap
	float tilemap_right, tilemap_left, tilemap_top, tilemap_bottom;
	Rect rect(0,0,0,0);

	tilemap_left = 0;
	tilemap_right = (float)(tilesize * width);
	tilemap_top = (float)(tilesize * height);
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

		left /= tilesize;
		right /= tilesize;
		top /= tilesize;
		bottom /= tilesize;

		rect.left = (int)floor(left);
		rect.right = (int)ceil(right);
		rect.top = (int)ceil(top);
		rect.bottom = (int)floor(bottom);
	}

	return rect;
}

Vector2<double> Tilemap::getTileCoordinates(float x, float y, int layer) const
{
	if (x >= 0 && y >= 0 && layer >= 0)
	{
		return Vector2<double>(x / tilesize, y / tilesize);
	}

	return Vector2<double>(-1,-1);
}

unsigned int Tilemap::getTile(uint x, uint y, uint z) const
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height) || (created == false))
	{
		return -1;
	}

	return tilemap[x][y][z];
}

void Tilemap::destroy()
{
	uint x,y;

    cout << "DESTROYING MAP WITH " << width << " " << height << " " << layers << endl;
	if (!tilemap)
	{
		return;
	}

	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
		    if (layers > 1)
                delete [] tilemap[x][y];
            else
                delete tilemap[x][y];
		}
		delete [] tilemap[x];
	}

	width = 0;
	height = 0;
	layers = 0;
	tilemap = 0;

	delete [] tilemap;
}

void Tilemap::render(Renderer &renderer, float camx, float camy, float zoom, float aspect, ObjectZ depth, int layer)
{
	uint x,y;

	if (!tileset)
	{
//		clog << "Warning: Cannot render tilemap: No tile set is set.\n" << flush;
		return;
	}

	if (tileset->getTileCount() == 0)
	{
	//	clog << "Warning: Cannot render tilemap: At least 1 texture must be in the tile set.\n" << flush;;
		return;
	}

	float xmin;
	float ymin;
	float xmax;
	float ymax;

	//calculate the first and last tiles to be rendered (optimization) so the render loop does not go through EVERY tile
	//if the map is 500x500, the loop must loop 250000 times with one layer without optimization, but with optimization,
	//it may have to loop just 100 times, more or less, depending on how close the cam is zoomed in

	if (aspect > 1.0f)
	{
		xmin = (camx - (zoom * aspect)) / tilesize;
		ymin = (camy - zoom) / tilesize;// * aspect;
		xmax = xmin + (((zoom * 2) * aspect) / tilesize) + 1;
		ymax = ymin + ((zoom * 2) / tilesize) + 1;
	}
	else
	{
		xmin = (camx - zoom) / tilesize;
		ymin = (camy - (zoom / aspect)) / tilesize;
		xmax = xmin + ((zoom * 2) / tilesize) + 1;
		ymax = ymin + (((zoom * 2) / aspect) / tilesize) + 1;
	}

	xmin -= 1.0f;
	xmax += 1.0f;

	ymin -= 1.0f;
	ymax += 1.0f;

	if (xmin < 0.0f) xmin = 0.0f;
	if (xmin > (float)width) xmin = (float)width;

	if (ymin < 0.0f) ymin = 0.0f;
	if (ymin > (float)height) ymin = (float)height;

	if (xmax < 0.0f) xmax = 0.0f;
	if (xmax > (float)width) xmax = (float)width;

	if (ymax < 0.0f) ymax = 0.0f;
	if (ymax > (float)height) ymax = (float)height;

	for (x = (uint)xmin; x < (uint)xmax; x++)
	{
		for (y = (uint)ymin; y < (uint)ymax; y++)
		{
		    if (tilemap[x][y][layer] == 0)
                continue;
            renderer.beginRender(depth, tileset->getTile(tilemap[x][y][layer]).texture);
			renderer.addVertex((double)x * tilesize, (double)y * tilesize + tilesize, 0, 0);
			renderer.addVertex((double)x * tilesize + tilesize, (double)y * tilesize + tilesize, 1, 0);
			renderer.addVertex((double)x * tilesize + tilesize, (double)y * tilesize, 1, 1);
			renderer.addVertex((double)x * tilesize, (double)y * tilesize, 0, 1);
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
	this->tileset = tileset;
}

void Tilemap::setTile(uint x, uint y, uint z, uint tile)
{
	if (!tileset)
	{
		return;
	}

	if ((x >= 0) && (x < width) && (y >= 0) && (y < height) && (z >= 0) && (z < layers))
	{
		//if ((tile >= 0) && (tile < tileset->getTileCount()))
		if (tile >= 0)
		{
			tilemap[x][y][z] = tile;
		}
	}
}

int Tilemap::getWidth()
{
	return width;
}

int Tilemap::getHeight()
{
	return height;
}

int Tilemap::getLayers()
{
	return layers;
}
