#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include "com_exceptions.h"

#include "gfx_display.h"


using namespace Hinage;
using namespace std;

Display::Display() : zoom(200.0), camX(0.0f), camY(0.0f)
{
	//set states
	displayState = 0;
}

//initialize opengl
void Display::initialize(int width, int height, bool fullscreen)
{
	clog << "Initializing display. Width: " << width << " Height: " << height << endl << flush;
	SDL_Init(SDL_INIT_VIDEO);

	//set color/depth buffer sizes sizes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (fullscreen)
	{
		clog << "- Setting video mode - fullscreen...\n" << flush;
		if (!SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_HWSURFACE | SDL_FULLSCREEN))
		{
			throw FatalException("ERROR: Setting video mode failed.");
		}
	}
	else
	{
		clog << "- Setting video mode - windowed...\n" << flush;
		if (!SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_HWSURFACE))
		{
			throw FatalException("ERROR: Display initialization failed.");
		}
	}

	aspect = (float)width / (float)height;
	clog << "- Aspect ratio: " << aspect << endl << flush;

	//enable opengl states
	glEnable(GL_DEPTH_TEST);	//enable depth testing
	glEnable(GL_TEXTURE_2D);	//enable texture mapping

	glShadeModel (GL_SMOOTH);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER,0.1f);
	glEnable(GL_ALPHA_TEST);

	//glDisable(GL_COLOR_MATERIAL);

	resize(width,height);

	clog << "OK: Display set up successfully.\n" << flush;
}

//resize the openGL scene
void Display::resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	if (width == 0)
	{
		width = 1;
	}

	this->width = width;
	this->height = height;

	glViewport(0,0,width,height);
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (displayState & DS_STRETCH_SCENE)
	{
		glOrtho(-zoom,zoom,-zoom,zoom,-1.0, 1000.0);
	}
	else
	{
		aspect = (float)width / (float)height;

		if (width > height)
		{
			glOrtho(-zoom * aspect, zoom * aspect, -zoom, zoom, -1.0, 1000.0);
		}
		else
		{
			glOrtho(-zoom, zoom, -zoom / aspect, zoom / aspect, -1.0, 1000.0);
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Display::setCamPosition(float x, float y)
{
	camX = x;
	camY = y;

	glLoadIdentity();
	glTranslatef(-camX,-camY,0);
}

void Display::setZoom(double value)
{
	zoom = value;
	if (zoom < 1.0)
	{
		zoom = 1.0;
	}

	resize(width, height);
}

void Display::beginRender()
{
	//set up the scene for rendering
	glLoadIdentity();
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glTranslatef(-camX,-camY,0);

	renderer.clearBuffer();
}

void Display::render()
{
	renderer.renderBuffer();
	SDL_GL_SwapBuffers();
}

void Display::setState(enum Display::DisplayState state, bool value)
{
	//enable state
	if (value)
	{
		displayState |= state;
		clog << "OK: Display state " << state << " enabled.\n" << flush;
	}
	//disable state
	else
	{
		displayState ^= displayState & state;
		clog << "OK: Display state " << state << " disabled.\n" << flush;
	}
}

//viewport width
float Display::getViewWidth()
{
	if (width > height)
	{
		return ((float)zoom * aspect * 2);
	}

	return ((float)zoom * 2);
}

//viewport height
float Display::getViewHeight()
{
	if (height > width)
	{
		return ((float)zoom / aspect * 2);
	}

	return ((float)zoom * 2);
}

//converts window coordinates to viewport coodinates (moves the origin to the center, and inverts the y-axis)
Vector2<double> Display::windowToViewport(Vector2<double> coord)
{
	Vector2<double> newcoord;

	//find the ratio between the viewport and window dimensions
	float xRatio = width / getViewWidth();
	float yRatio = height / getViewHeight();

	if (width > height)
	{
		newcoord.setX((coord.getX() / xRatio) - (zoom * aspect));
		newcoord.setY(-((coord.getY() / yRatio) - zoom));
	}
	else
	{
		newcoord.setX((coord.getX() / xRatio) - zoom);
		newcoord.setY(-((coord.getY() / yRatio) - (zoom * aspect)));
	}

	return newcoord;
}

//converts from window coordinates to coordinates in the environment. Same as window to viewport, except it takes the camera position into account.
Vector2<double> Display::windowToEnvironment(Vector2<double> coord)
{
	return (windowToViewport(coord) + Vector2<double>(camX, camY));
}


Vector2<double> Display::environmentToWindow(Vector2<double> coord)
{
	float xRatio = width / getViewWidth();
	float yRatio = height / getViewHeight();

	Vector2<double> newcoord;

	newcoord = coord;
	if (width > height)
	{
		newcoord.setX((coord.getX() / xRatio) + camX - (zoom * aspect));
		newcoord.setY((-coord.getY() / yRatio) + camY + zoom);
	}
	else
	{
		newcoord.setX((coord.getX() / xRatio) + camX - zoom);
		newcoord.setY((-coord.getY() / yRatio) + camY + (zoom * aspect));
	}

	return newcoord;
}
