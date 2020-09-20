#include <iostream>
#include "gfx_tileset.h"

using namespace Hinage;
using namespace std;

//constructor/destructor
Tileset::Tileset() : tileCount(0)
{
    for (int i = 0; i < MAX_TILES; i++)
    {
        tiles[i].id = -1;
        tilesOrg[i].id = -1;
    }
    last = -1;
    first = MAX_TILES + 1;
}

Tileset::~Tileset()
{
}

Tileset::iterator Tileset::begin()
{
    if (first <= last)
    {
        return Tileset::iterator(&tilesOrg[first]);
    }

    return Tileset::iterator();
}

Tileset::iterator Tileset::end()
{
    if (first <= last)
    {
        return Tileset::iterator(&tilesOrg[last+1]);
    }

    return Tileset::iterator();
}

//load a texture from a file to add to the tileset
bool Tileset::addTile(Texture * texture, int block, int nextTile, int delay, string textureFile, int id)
{
    //if no ID is specified, find the first free slot
    if (id == -1)
    {
        //id = tileCount;
        for (int i = 0; i < MAX_TILES; i++)
        {
            if (tilesOrg[i].id == -1)
            {
                id = i;
                break;
            }
        }
    }
	tilesOrg[id].id = id;

	tilesOrg[id].texture = texture;
	tilesOrg[id].block = block;
	tilesOrg[id].nextTile = nextTile;
	tilesOrg[id].delay = delay;
	tilesOrg[id].counter = 0;

    if (id < first)
    {
        first = id;
    }
    if (id > last)
    {
        if (id != last+1)
        {
            tilesOrg[last+1].id = -1;
        }
        last = id;
        tilesOrg[last+1].id = -2;
    }

	//textureFiles.push_back(textureFile);
	textureFiles[id] = textureFile;

	tileCount++;

	for (int i = 0; i < tileCount; i++)
	{
		tiles[i] = tilesOrg[i];
	}

	clog << "OK: Tile added to tileset successfully.\n" << flush;

	return true;
}

/*void Tileset::updateTiles()
{
	for (int i = 0; i < tileCount; i++)
	{
		tiles[i]->counter++;

		if ((tiles[i]->counter >= tiles[i]->delay) && (tiles[i]->delay > 0) && (tiles[i]->nextTile >= 0))
		{
			tiles[i] = tilesOrg[tiles[i]->nextTile];
			tiles[i]->counter = 0;
		}

	}
}*/

Tileset::Tile Tileset::getTile(int index) const
{
	 //if ((index >= 0) && (index <= last))
	 if (tilesOrg[index].id != -1)
	 {
		 return tilesOrg[index];
	 }
	 else
	 {
	    if ((tilesOrg[first].id != -1) && first <= last)
	    {
            return tilesOrg[first];
	    }
	    else
	    {
            Tile nulltile;
            memset(&nulltile,0,sizeof(Tile));

            return nulltile;
	    }
	}
}

string Tileset::getTextureFileName(int tileID)
{
    return textureFiles[tileID];
}


