/*!
	\file mapeditgame.cpp

	\author Hallgeir Lien

	Description:
*/

#include <iostream>
#include <fstream>
#include <engine/com_objectfactory.h>
#include "mapeditgame.h"

using namespace std;
using namespace Hinage;

/*
    class MapEditGame
*/

void MapEditGame::onInit()
{
    try
    {
        loadResources("data/textures", TEXTURE);
        loadResources("data/sprites", SPRITE);
        loadResources("data/tilesets", TILESET);
        loadResources("data/objects", OBJECT);
        loadResources("data/fonts", FONT);
    }
    catch (FatalException &e)
    {
        stop();

        return;
    }

    //create a new mapstate
    mapState = new MapEditState(*this);

    setGameState(mapState);
}

int MapEditGame::getMapLayers()
{
    return mapState->gamemap.getLayerCount();
}

int MapEditGame::getMapWidth()
{
    return mapState->gamemap.getWidth();
}

int MapEditGame::getMapHeight()
{
    return mapState->gamemap.getHeight();
}


void MapEditGame::setDrawingLayer(int layer)
{
    mapState->drawingLayer = layer;
}

void MapEditGame::setBackground(std::string name)
{
    mapState->gamemap.setBackground(name);
}

void MapEditGame::createEmptyMap(int width, int height, int layers, int tileSize)
{
    mapState->gamemap.createEmpty(width, height, layers, tileSize);
}

void MapEditGame::loadMap(std::string path)
{
    mapState->gamemap.createFromFile(path, false);
}

void MapEditGame::saveMap(std::string path)
{
    mapState->gamemap.saveToFile(path);
}

void MapEditGame::resizeMap(int width, int height, int layers, int tileSize)
{
    int oldWidth = mapState->gamemap.getWidth();
    int oldHeight = mapState->gamemap.getHeight();
    int oldLayers = mapState->gamemap.getLayerCount();
   // int oldTilesize = mapState->gamemap.getTileSize();

    if (width > 0 && height > 0 && layers > 0 && tileSize > 0)
    {
        //Backup the old map
        vector<vector<vector<unsigned int> > > backup;

        for (int x = 0; x < oldWidth; x++)
        {
            backup.push_back(vector<vector<unsigned int> >());
            for (int y = 0; y < oldHeight; y++)
            {
                backup[x].push_back(vector<unsigned int>());
                for (int z = 0; z < oldLayers; z++)
                {
                    backup[x][y].push_back(mapState->gamemap.getTile(x,y,z));
                }
            }
        }

        //Create a new map with the new sizes
        mapState->gamemap.createEmpty(width, height, layers, tileSize, true);

        //Fill the map with the data from the backup
        for (int x = 0; x < oldWidth && x < width; x++)
        {
            for (int y = 0; y < oldHeight && y < height; y++)
            {
                for (int z = 0; z < oldLayers && z < layers; z++)
                {
                    mapState->gamemap.setTile(x,y,z, backup[x][y][z]);
                }
            }
        }
    }
    else
    {
        throw Exception("Invalid map dimensions.");
    }
}

/*
    class MapEditState
*/

MapEditState::MapEditState(Game &game) : MapState(game)
{
    currentTile = 0;
    currentObject = "";
    currentItemType = OBJECT;
    drawingLayer = 0;
    deleteObject = false;
    snap = 1;
}

MapEditState::~MapEditState()
{

}

void MapEditState::init()
{
    //try loading the recent map
    try
    {
        //get the filename
        ifstream instream("recent");
        string filename;
        if (!instream.is_open())
        {
            throw IOException("Could not open the config file with the recent filename.");
        }
        instream >> filename;
        instream.close();

        gamemap.createFromFile(filename, false);
    }
    catch (Exception &e)
    {
        //didn't work. create a empty one
        gamemap.createEmpty(100,30,3,32);
    }
}

void MapEditState::destroy()
{
}

void MapEditState::render(double frametime)
{
    gamemap.render();
}

void MapEditState::update(double frametime)
{
    interactTimer.update();
}

void MapEditState::handleEvents(double frametime)
{
    InputManager &input = gameInstance.getInputManager();
    Display &disp = gameInstance.getDisplay();
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
        //map scrolling
		case SDL_MOUSEMOTION:
			{
				if (input.mouseButtonDown(3))
				{
					float x = (float)event.motion.xrel / ((float)disp.getWidth() / disp.getViewWidth());
					float y = (float)event.motion.yrel / ((float)disp.getHeight() / disp.getViewHeight());

					float cx = disp.getCamX();
					float cy = disp.getCamY();

					disp.setCamPosition(cx - x, cy + y);
				}
			}
			break;

		case SDL_QUIT:
			{
				gameInstance.stop();
			}
			break;
		}
	}

    Vector2<double> coord;

	//perform operations on the map
	if (input.mouseButtonDown(1, &coord))
	{
        coord = disp.windowToEnvironment(coord);
        if (deleteObject)
        {
            try
            {
                gamemap.deleteObjectAt(coord.getX(), coord.getY());
            }
            catch (Exception &e)
            {
            }
        }
        else
        {
            switch (currentItemType)
            {
            case TILE:
                if (currentTile >= 0)
                {
                    gamemap.setTileAt(coord.getX(), coord.getY(), drawingLayer, currentTile);
                }
                break;

            case OBJECT:
                if (interactTimer.hasReached(0.1))
                {
                    //snap to the grid
                    coord.set((int)(coord.getX() / snap) * snap,(int)(coord.getY() / snap) * snap);
                    try
                    {
                        bool objectAtPosition = false;
                        for (size_t i = 0; i < gamemap.getObjectCount(); i++)
                        {
                            if (gamemap.getObject(i).getX() == coord.getX() && gamemap.getObject(i).getY() == coord.getY())
                            {
                                objectAtPosition = true;
                            }
                        }
                        if (!objectAtPosition)
                        {
                            gamemap.createObject( currentObject, coord.getX(), coord.getY(), false);
                        }
                    }
                    catch (Exception &e)
                    {

                    }
                    interactTimer.reset();
                }
                break;
            default:
                break;
            }
        }
	}
}

void MapEditGame::setTileset(std::string name)
{
    mapState->gamemap.setTileset(name);
}

string MapEditGame::getTilesetName()
{
    return mapState->gamemap.getTilesetName();
}

void MapEditGame::setTileMode(int itemID)
{
    mapState->currentItemType = TILE;
    mapState->currentTile = itemID;
    mapState->deleteObject = false;
}

void MapEditGame::setObjectMode(std::string name)
{
    mapState->currentItemType = OBJECT;
    mapState->currentObject = name;
    mapState->deleteObject = false;
}

void MapEditGame::setDeleteMode()
{
    mapState->deleteObject = true;
}

void MapEditGame::setSnap(int value)
{
    mapState->snap = value;
}


size_t MapEditGame::getScriptCount(ScriptTypeEnum type)
{
    switch (type)
    {
        case INCLUDE:
            return mapState->gamemap.includeScripts.size();
        case INIT:
            return mapState->gamemap.initScripts.size();
        case UPDATE:
            return mapState->gamemap.updateScripts.size();
        case SHUTDOWN:
            return mapState->gamemap.shutdownScripts.size();
    }

    return 0;
}

string MapEditGame::getScript(int id, ScriptTypeEnum type)
{
    switch (type)
    {
        case INCLUDE:
            return mapState->gamemap.includeScripts[id];
        case INIT:
            return mapState->gamemap.initScripts[id];
        case UPDATE:
            return mapState->gamemap.updateScripts[id];
        case SHUTDOWN:
            return mapState->gamemap.shutdownScripts[id];
    }

    return "";
}

void MapEditGame::addScript(std::string script, ScriptTypeEnum type)
{
    if (script.length() == 0)
    {
        return;
    }

    switch (type)
    {
        case INCLUDE:
            mapState->gamemap.includeScripts.push_back(script);
            break;
        case INIT:
            mapState->gamemap.initScripts.push_back(script);
            break;
        case UPDATE:
            mapState->gamemap.updateScripts.push_back(script);
            break;
        case SHUTDOWN:
            mapState->gamemap.shutdownScripts.push_back(script);
            break;
    }
}

void MapEditGame::removeScript(std::string script, ScriptTypeEnum type)
{
    for (size_t i = 0; i < getScriptCount(type); i++)
    {
        switch (type)
        {
            case INCLUDE:
                if (!(mapState->gamemap.includeScripts[i].compare(script)))
                {
                    mapState->gamemap.includeScripts.erase(mapState->gamemap.includeScripts.begin()+i);
                }
                break;

            case INIT:
                if (!(mapState->gamemap.initScripts[i].compare(script)))
                {
                    mapState->gamemap.initScripts.erase(mapState->gamemap.initScripts.begin()+i);
                }
                break;

            case UPDATE:
                if (!(mapState->gamemap.updateScripts[i].compare(script)))
                {
                    mapState->gamemap.updateScripts.erase(mapState->gamemap.updateScripts.begin()+i);
                }
                break;
            case SHUTDOWN:
                if (!(mapState->gamemap.shutdownScripts[i].compare(script)))
                {
                    mapState->gamemap.shutdownScripts.erase(mapState->gamemap.shutdownScripts.begin()+i);
                }
                break;
        }
    }
}


