#pragma once

#include <iostream>
#include <map>

//define the import/export macro for Win32
#ifndef __IMPORTEXPORT
	/*#ifdef WIN32
		#ifdef ENGINE
			#define __IMPORTEXPORT __declspec (dllexport)
		#else
			#define __IMPORTEXPORT __declspec (dllimport)

		#endif
	#else*/
		#define __IMPORTEXPORT
//	#endif
#endif

//GL_CLAMP_TO_EDGE works best for 2D sprite textures but is not defined in Microsoft's OpenGL header.
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

/*!
	\namespace Hinage
	The namespace for the engine. Contains every engine class, typedef, function, enum and everything you need.
*/
namespace hiage
{
	/*typedef struct _Coordinate
	{
		double x, y;

		_Coordinate() : x(0), y(0) {}
	} Coordinate;*/

	#ifndef uint
	typedef unsigned int uint;
	#endif

	#ifndef ubyte
	typedef unsigned char ubyte;
	#endif

	#ifndef byte
	typedef signed char byte;
	#endif

	typedef std::map<std::string, std::string> ObjectList;

	enum class ResourceTypeEnum
	{
		SPRITE = 1,
		OBJECT = 2,
		TEXTURE = 3,
		TILESET = 4,
		FONT = 5,
		TILE = 6

	};
}
