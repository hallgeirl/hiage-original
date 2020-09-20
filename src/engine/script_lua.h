/*!
	\file util_vector2.h

	\author Hallgeir Lien

	Contains declaration of the LuaVM scripting class.
*/

#pragma once

#include <iostream>

#include <map>
#include <vector>
#include <string>
extern "C"
{
	#include <lua50/lua.h>
	#include <lua50/lualib.h>
	#include <lua50/lauxlib.h>
}

#define LUA_INCLUDE_HPP_INCLUDED
#include "util_vector2.h"

namespace Hinage
{
	/*!
		LuaVM is a wrapper class for the scripting environment. It loads and executes scripts, simply put.
	*/
	class LuaVM
	{
	private:
		//Typedefs for storing scripts
		typedef std::string LuaProgram;
		typedef std::pair<std::string, LuaProgram> LuaPair;
		typedef std::map<std::string, LuaProgram> LuaScriptContainer;

		LuaScriptContainer scripts;
		lua_State *vm;

		//initialize lua
		void initialize();

	public:
		/*!
			Constructs a LuaVM object and initializes it (loads all libraries, etc.)
		*/
		LuaVM();
		LuaVM(LuaVM &obj);
		~LuaVM();

		/*!
			Loads a script from file, and assigns it a name.
			\param name Name to use to call the script later. The names must be unique.
			\param path Path to the .lua-file to load.
		*/
		void loadScript(std::string name, std::string path);

		/*!
			Runs a loaded script.
			\param name Name of the script to call. Name is chosen when loading (see LuaVM::loadScript())
		*/
		void runScript(std::string name);
		void runFile(std::string path);

		/*!
			Executes a single line of Lua-code.
		*/
		void executeLine(std::string line);

		/*!
			Returns a pointer to the current Lua state. The only time this should be required is when registering new classes with luabind.
		*/
		lua_State *getVm();
	};
}
