#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include "windows.h"

using namespace hiage;

UIWindow::UIWindow()
{
}

bool UIWindow::create(float x, float y, float width, float height, float borderWidth, ubyte borderRed, ubyte borderGreen, ubyte borderBlue, ubyte borderAlpha, ubyte red, ubyte green, ubyte blue, ubyte alpha)
{
	this->_x = x;
	this->_y = y;
	this->_width = width;
	this->_height = height;
	this->_borderWidth = borderWidth;
	this->_borderRed = borderRed;
	this->_borderGreen = borderGreen;
	this->_borderBlue = borderBlue;
	this->_borderAlpha = borderAlpha;
	this->_red = red;
	this->_green = green;
	this->_blue = blue;
	this->_alpha = alpha;

	_initialized = true;

	return true;
}

void UIWindow::render()
{
	if (!_initialized)
	{
		return;
	}

	glBegin(GL_QUADS);

	//TODO: finish method
	//glVertex3f

	glEnd();
}
