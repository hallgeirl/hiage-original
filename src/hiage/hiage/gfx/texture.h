/*
	Header for the Texture Class
*/

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "../core/typedefs.h"

namespace hiage
{
	class __IMPORTEXPORT Texture
	{
		friend class CTilemap;
	public:
		Texture() : _textureID(0), _loaded(false), _height(0), _width(0), _bits(0), _filename(nullptr) {}
		~Texture();

		//create a texture out of pre-loaded image data, width and height, to add support for other filetypes
		void createTexture(const unsigned char * imageData, short width, short height, char cBits);

		//load a Targa (.TGA) bitmap and use it as a texture
		void loadTexture(const char * path);

		int getTextureID() const;
		void select() const;

		short getWidth() const { return _width; }
		short getHeight() const { return _height; }

	private:
		unsigned int _textureID;
		short _width;
		short _height;
		char _bits;
		char * _filename;	//for checking for double textures
		bool _loaded;
		//unsigned char * imageData;	//image data (stored in case it is needed for stuff like collision detection)
	};
}
