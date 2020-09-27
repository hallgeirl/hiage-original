#pragma once

#pragma warning(push, 0)
extern "C"
{
	#include <lua50/lualib.h>
	#include <lua50/lua.h>
	#include <lua50/lauxlib.h>
}

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#pragma warning(pop)