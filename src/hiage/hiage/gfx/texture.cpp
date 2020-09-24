#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <fstream>
#include <iostream>
#include "texture.h"
#include "../util/exceptions.h"

using namespace std;
using namespace hiage;

Texture::~Texture()
{
}

//create a pre-loaded texture
void Texture::createTexture(const unsigned char * imageData, short width, short height, char bits)
{
	clog << "- Creating texture... Width: " << width << " Height: " << height << " Bits per pixel: " << (int)bits << endl << flush;

	if (loaded)
	{
		glDeleteTextures(1,&this->textureID);
		loaded = false;
	}

	this->width = width;
	this->height = height;
	this->bits = bits;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/*glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	//glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	clog << "- Creating " << (int)bits << " bit texture...\n" << flush;
	if (bits == 32)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		delete [] imageData;

	}
	else if (bits == 24)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		delete [] imageData;
	}
	else
	{
		delete [] imageData;
		throw Exception("ERROR: Unsupported bit depth. Only 32 and 24 bits are supported.");
	}

	//set the texture environment parameters
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	clog << "OK: Texture was created successfully.\n" << flush;

	loaded = true;
}

//load a texture from a TGA bitmap
void Texture::loadTexture(const char * path)
{
	ifstream fStream;
	char imageType;
	char imageDesc;

	clog << "- Loading texture from Targa " << path << "...\n" << flush;

	//image orientation
	bool leftToRight = true;	//pixels are arranged from left to right
	bool bottomToTop = true;	//pixels are arranged from bottom to top

	unsigned char * bitmapData;

	fStream.open(path,ios::in | ios::binary);
	if (!fStream.is_open())
	{
		throw IOException("ERROR: Could not open file.");
	}

	//skip the Image ID and color map type (1 byte each)
	fStream.seekg(2,ios::cur);

	//read and check Image Type
	fStream.read(&imageType,1);
	if (imageType != 2)
	{
		throw Exception("ERROR: Only 24/32 bit uncompressed Targas are supported.");
	}

	//skip the color map specifications and image origin (5+4 bytes)
	fStream.seekg(9,ios::cur);

	//read width, height and pixel depth
	fStream.read((char *)&width,2);
	fStream.read((char *)&height,2);
	fStream.read(&bits,1);
	clog << "- Dimensions:\n" << "  - Width: " << width << " Height: " << height << " Bit depth: " << (int)bits << endl << flush;
	if (bits != 32)
	{
		throw Exception("ERROR: Only 32 bit targas are supported.");
	}

	//read the image descriptor
	fStream.read(&imageDesc,1);
	if (imageDesc & 16)
	{
		leftToRight = false;
	}
	if (imageDesc & 32)
	{
		bottomToTop = false;
	}

	/*
		color map and image ID goes here
	*/

	int x,y;	//counters
	int chans = bits / 8;
	int rowlength = width * chans;
	bitmapData = new unsigned char[rowlength * height];
	//imageData = bitmapData;

	//image data
	if (bottomToTop)
	{
		//read the image from bottom to top
		if (leftToRight)
		{
			//read the pixels in the row from left to right
			for (y = height - 1; y >= 0; y--)
			{
				fStream.read((char *)bitmapData + (y * rowlength), rowlength);
			}
		}
		else
		{
			//read the pixels in the row from right to left
			for (y = height - 1; y >= 0; y--)
			{
				for (x = rowlength - chans - 1; x >= 0; x -= chans)
				{
					fStream.read((char *)bitmapData + (y * rowlength) + x, chans);
				}
			}
		}
	}
	else
	{
		//read the image from top to bottom
		if (leftToRight)
		{
			//read the pixels in the row from left to right
			fStream.read((char *)bitmapData, height * rowlength);
		}
		else
		{
			//read the pixels in the row from right to left
			for (y = 0; y < height; y++)
			{
				for (x = rowlength - chans - 1; x >= 0; x -= chans)
				{
					fStream.read((char *)bitmapData + (y * rowlength) + x, chans);
				}
			}
		}
	}

	fStream.close();

	//swap Red and Blue bits
	for (x = 0; x < rowlength * height; x+=chans)
	{
		//store the red bits
		char red = bitmapData[x];
		bitmapData[x] = bitmapData[x+2];
		bitmapData[x+2] = red;
	}

	try
	{
		createTexture(bitmapData, width, height, bits);
	}
	catch (...)
	{
		delete [] bitmapData;
	}


	clog << "OK: Texture loaded and created successfully.\n" << flush;
}


//select the texture for rendering
void Texture::select()
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}

int Texture::getTextureID()
{
	return textureID;
}


