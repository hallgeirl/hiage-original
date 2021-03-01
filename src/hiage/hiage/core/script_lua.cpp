/*!
	\file script_lua.cpp

	\author Hallgeir Lien

	Implementation of the LUA script VM.
*/

#include <fstream>
#include "script_lua.h"
#include "../util/exceptions.h"

//includes used to register the classes
#include "map.hpp"
#include "entity.hpp"
#include "game.hpp"
#include "../util/timer.h"

#include "../lua-includes.h"

#include <filesystem>

using namespace std;
using namespace hiage;

LuaVM::LuaVM(const std::string& dataRoot) : dataRoot(dataRoot)
{
	initialize();
}

LuaVM::~LuaVM()
{
	lua_close(vm);
}

void LuaVM::initialize()
{
	clog << "Initializing LUA VM..." << endl;
	vm = lua_open();

	lua_baselibopen(vm);
	lua_iolibopen(vm);
	lua_strlibopen(vm);
	lua_mathlibopen(vm);
	lua_tablibopen(vm);
	

	/*
	Keeping the code commented out for now, for reference
	*/

	//luabind::open(vm);

	//register engine classes with LUA
	//game class
	
	/*luabind::module(vm)
	[
		luabind::class_<Game>("Game")
			.def("stop", &Game::stop)
			.def("timefactor", &Game::setTimeFactor)
			.def("timefactor", &Game::getTimeFactor)
			.def("createfont", &Game::createFont)
			.def("print", &Game::printText)
			.def("drawtexture", &Game::drawTexture)
	];

    //display class
	luabind::module(vm)
	[
		luabind::class_<Display>("Screen")
			.def("camera", &Display::setCamPosition)
			.def("zoom", &Display::setZoom)
            .def("zoom", &Display::getZoom)
            .def("aspect", &Display::getAspectRatio)
            .def("camerax", &Display::getCamX)
            .def("cameray", &Display::getCamY)
	];

    //input class
	luabind::module(vm)
	[
        luabind::class_<InputManager>("Input")
            .def("keydown", &InputManager::keyDown)
	];

	//audio class
	luabind::module(vm)
	[
        luabind::class_<AudioManager>("Audio")
            .def("play", &AudioManager::playWav)
            .def("playmusic", &AudioManager::playOgg)
            .def("stopmusic", &AudioManager::stopOgg)
	];

	//map class
 	luabind::module(vm)
	[
		luabind::class_<Map>("Map")
			//.def("spawn", (PhysicalEntity & (Map::*)(string,double,double))&Map::createObject)
			//.def("destroy", &Map::deleteObject)
            //.def("getobject", &Map::getObject)

            //.def("load", (void (Map::*)(string))&Map::createFromFile)
            .def("load", &Map::queueCreateMap)
            //.def("unload", &Map::destroy)
            .def("tilesize", &Map::getTileSize)
            .def("width", &Map::getWidth)
            .def("height", &Map::getHeight)
            .def("setflag", &Map::setFlag)
	];*/

    //entity class
/*	luabind::module(vm)
	[
		luabind::class_<PhysicalEntity>("Entity")
			//get/set position
			//needs a cast to the correct function because of overloading
			.def("position", (void (PhysicalEntity::*)(double,double))&PhysicalEntity::setPosition)
			.def("x", &PhysicalEntity::getX)
			.def("y", &PhysicalEntity::getY)
			.def("x", &PhysicalEntity::setX)
			.def("y", &PhysicalEntity::setY)
			//get/set speed & accellerate
			.def("velocity", (void (PhysicalEntity::*)(double,double))&PhysicalEntity::setVelocity)
			.def("speed", (double (PhysicalEntity::*)())&PhysicalEntity::getSpeed)
			.def("velx", &PhysicalEntity::getXVelocity)
			.def("vely", &PhysicalEntity::getYVelocity)
			.def("velx", &PhysicalEntity::setXVelocity)
			.def("vely", &PhysicalEntity::setYVelocity)
			.def("accel", (void (PhysicalEntity::*)(double,double,double))&PhysicalEntity::accellerate)
            //get hitboxes
			.def("hbleft", &PhysicalEntity::getHitboxLeft)
			.def("hbright", &PhysicalEntity::getHitboxRight)
			.def("hbtop", &PhysicalEntity::getHitboxTop)
			.def("hbbottom", &PhysicalEntity::getHitboxBottom)
            .def("cancollide", &PhysicalEntity::setCollision)

            //animation stuff
			.def("playanimation", &PhysicalEntity::playAnimation)
            .def("flipx", &PhysicalEntity::flipx)
            .def("flipy", &PhysicalEntity::flipy)

			//true if the object collided with another object
            .def_readonly("collided", &PhysicalEntity::collided)
            .def("type", &PhysicalEntity::getType)
            .def("name", &PhysicalEntity::getName)

    ];
	*/

    //timer class
    /*luabind::module(vm)
    [
        luabind::class_<Timer>("Timer")
            .def(luabind::constructor< >())
            .def(luabind::constructor<bool>())
            .def("reset", &Timer::reset)
            .def("update", &Timer::update)
            .def("reached", &Timer::hasReached)
            .def("get", &Timer::getTime)
            .def("stop", &Timer::stop)
            .def("start", &Timer::start)
    ];

    //vector class (type double)
    luabind::module(vm)
    [
        luabind::class_<Vector2<double> >("Vector")
            .def(luabind::constructor< >())
            .def(luabind::constructor<double, double>())
            .def("x", &Vector2<double>::getX)
            .def("y", &Vector2<double>::getY)
            .def("x", &Vector2<double>::setX)
            .def("y", &Vector2<double>::setY)
            .def("set", (void (Vector2<double>::*)(double,double))&Vector2<double>::set)
    ];

    //font class
    luabind::module(vm)
    [
        luabind::class_<Font>("Font")
            //.def("print", &Font::renderText)
    ];*/
}

void LuaVM::loadScript(std::string name, std::string path)
{
	LuaScriptContainer::iterator itr;
	char buffer[1000];

	itr = scripts.find(name);
	if (itr != scripts.end())
	{
		throw Exception(string("ERROR: Could not load script ") + path + ". Name " + name + " already taken.");
	}

	ifstream file(path.c_str());
	if (!file.is_open())
	{
		throw IOException(string("ERROR: Could not open script file ") + path);
	}

	//load the script lines from a file
	scripts.insert(LuaPair(name, LuaProgram()));

	itr = scripts.find(name);


	while (file.getline(buffer, 1000))
	{
		itr->second += buffer + '\n';
	}
}

void LuaVM::runScript(std::string name)
{
	LuaScriptContainer::iterator itr;

	itr = scripts.find(name);

	if (itr == scripts.end())
	{
		throw Exception(string("ERROR: Could not find script ") + name);
	}

	lua_dostring(vm, itr->second.c_str());
}

void LuaVM::runFile(std::string path)
{
	std::filesystem::path root = dataRoot;
	auto scriptPath = root / path;
    lua_dofile(vm, scriptPath.string().c_str());
}



void LuaVM::executeLine(std::string line)
{
	lua_dostring(vm, line.c_str());
}

lua_State * LuaVM::getVm()
{
	return vm;
}
