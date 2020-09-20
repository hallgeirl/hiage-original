#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include "ui_windows.h"

using namespace Hinage;

UIWindow::UIWindow()
{
}

bool UIWindow::create(float x, float y, float width, float height, float borderWidth, ubyte borderRed, ubyte borderGreen, ubyte borderBlue, ubyte borderAlpha, ubyte red, ubyte green, ubyte blue, ubyte alpha)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->borderWidth = borderWidth;
	this->borderRed = borderRed;
	this->borderGreen = borderGreen;
	this->borderBlue = borderBlue;
	this->red = red;
	this->green = green;
	this->blue = blue;
	this->alpha = alpha;

	initialized = true;

	return true;
}

void UIWindow::render()
{
	if (!initialized)
	{
		return;
	}

	glBegin(GL_QUADS);

	//TODO: finish method
	//glVertex3f

	glEnd();
}
