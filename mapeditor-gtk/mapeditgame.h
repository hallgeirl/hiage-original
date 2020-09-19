/*!
	\file mapeditgame.h

	\author Hallgeir Lien

	Description:
*/

#pragma once

#include <engine/game_map.h>
#include <engine/util_fonts.h>
#include <engine/util_timer.h>
#include <engine/com_typedefs.h>

/*
    The game state we'll use in the editor
*/
class MapEditState : public Hinage::MapState
{
    friend class MapEditGame;
private:
    Hinage::Timer interactTimer;

    int drawingLayer;   //the layer to draw to
    int currentTile;    //the current tile to draw
    int snap;           //snap to a grid with this size
    std::string currentObject;
    bool deleteObject;
    Hinage::ResourceTypeEnum currentItemType;


public:
    MapEditState(Hinage::Game &game);

    virtual ~MapEditState();
	virtual void init();
	virtual void destroy();

	virtual void handleEvents(double frametime);
	virtual void update(double frametime);
	virtual void render(double frametime);
};

/*
    Main game object
*/
class MapEditGame : public Hinage::Game
{
private:
    MapEditState * mapState;

protected:
    virtual void onInit();

public:
    enum ScriptTypeEnum
    {
        INCLUDE = 0,
        INIT = 1,
        UPDATE = 2,
        SHUTDOWN = 3
    };
    void setDrawingLayer(int layer);

    //some interface functions to let the main window have access to the map
    void setBackground(std::string name);
    int getMapLayers();
    int getMapWidth();
    int getMapHeight();

    void createEmptyMap(int width, int height, int layers, int tileSize);
    void loadMap(std::string path);
    void saveMap(std::string path);

    void setTileset(std::string name);
    std::string getTilesetName();

    void setTileMode(int tileID);
    void setObjectMode(std::string name);
    void setDeleteMode();
    void setSnap(int value);

    void resizeMap(int width, int height, int layers, int tileSize);

    size_t      getScriptCount(ScriptTypeEnum type);
    std::string getScript(int id, ScriptTypeEnum type);
    void        addScript(std::string script, ScriptTypeEnum type);
    void        removeScript(std::string script, ScriptTypeEnum type);
};
