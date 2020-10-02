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
		Texture() : textureID(-1), loaded(false), height(0), width(0), bits(0), filename(nullptr) {}
		~Texture();

		//create a texture out of pre-loaded image data, width and height, to add support for other filetypes
		void createTexture(const unsigned char * imageData, short width, short height, char cBits);

		//load a Targa (.TGA) bitmap and use it as a texture
		void loadTexture(const char * path);

		int getTextureID();
		void select();

		short getWidth() { return width; }
		short getHeight() { return height; }

	private:
		unsigned int textureID;
		short width;
		short height;
		char bits;
		char * filename;	//for checking for double textures
		bool loaded;
		//unsigned char * imageData;	//image data (stored in case it is needed for stuff like collision detection)
	};
}
