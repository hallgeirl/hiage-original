#include <iostream>
#include "../sdl-includes.h"
#include "../util/exceptions.h"

#include "display.h"


using namespace hiage;
using namespace std;

Display::Display() : _zoom(200.0), _camX(0.0f), _camY(0.0f), _aspect(1), _height(768), _width(1024), _window(nullptr)
{
	//set states
	_displayState = 0;
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
		this->_window = SDL_CreateWindow("hiage", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
		if (!_window)
		{
			throw FatalException("ERROR: Setting video mode failed.");
		}
	}
	else
	{
		clog << "- Setting video mode - windowed...\n" << flush;
		this->_window = SDL_CreateWindow("hiage", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
		if (!_window)
		{
			throw FatalException("ERROR: Display initialization failed.");
		}
	}

	SDL_GL_CreateContext(this->_window);

	_aspect = (double)width / (double)height;
	clog << "- Aspect ratio: " << _aspect << endl << flush;

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

	this->_width = width;
	this->_height = height;

	glViewport(0,0,width,height);
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (_displayState & DisplayState::DS_STRETCH_SCENE)
	{
		glOrtho(-_zoom,_zoom,-_zoom,_zoom,-1.0, 1000.0);
	}
	else
	{
		_aspect = (double)width / (double)height;

		if (width > height)
		{
			glOrtho(-_zoom * _aspect, _zoom * _aspect, -_zoom, _zoom, -1.0, 1000.0);
		}
		else
		{
			glOrtho(-_zoom, _zoom, -_zoom / _aspect, _zoom / _aspect, -1.0, 1000.0);
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Display::setCamPosition(double x, double y)
{
	_camX = x;
	_camY = y;

	glLoadIdentity();
	glTranslated(-_camX,-_camY,0);
}

void Display::setZoom(double value)
{
	_zoom = value;
	if (_zoom < 1.0)
	{
		_zoom = 1.0;
	}

	resize(_width, _height);
}

void Display::beginRender()
{
	//set up the scene for rendering
	glLoadIdentity();
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glTranslated(-_camX,-_camY,0);

	_renderer.clearBuffer();
}

void Display::render()
{
	_renderer.renderBuffer();
	SDL_GL_SwapWindow(this->_window);
}

void Display::setState(DisplayState state, bool value)
{
	//enable state
	if (value)
	{
		_displayState |= state;
		clog << "OK: Display state " << state << " enabled.\n" << flush;
	}
	//disable state
	else
	{
		_displayState ^= _displayState & state;
		clog << "OK: Display state " << state << " disabled.\n" << flush;
	}
}

//viewport width
double Display::getViewWidth()
{
	if (_width > _height)
	{
		return (_zoom * _aspect * 2);
	}

	return (_zoom * 2);
}

//viewport height
double Display::getViewHeight()
{
	if (_height > _width)
	{
		return (_zoom / _aspect * 2);
	}

	return (_zoom * 2);
}

//converts window coordinates to viewport coodinates (moves the origin to the center, and inverts the y-axis)
Vector2<double> Display::windowToViewport(Vector2<double> coord)
{
	Vector2<double> newcoord;

	//find the ratio between the viewport and window dimensions
	double xRatio = _width / getViewWidth();
	double yRatio = _height / getViewHeight();

	if (_width > _height)
	{
		newcoord.setX((coord.getX() / xRatio) - (_zoom * _aspect));
		newcoord.setY(-((coord.getY() / yRatio) - _zoom));
	}
	else
	{
		newcoord.setX((coord.getX() / xRatio) - _zoom);
		newcoord.setY(-((coord.getY() / yRatio) - (_zoom * _aspect)));
	}

	return newcoord;
}

//converts from window coordinates to coordinates in the environment. Same as window to viewport, except it takes the camera position into account.
Vector2<double> Display::windowToEnvironment(Vector2<double> coord)
{
	return (windowToViewport(coord) + Vector2<double>(_camX, _camY));
}


Vector2<double> Display::environmentToWindow(Vector2<double> coord)
{
	double xRatio = _width / getViewWidth();
	double yRatio = _height / getViewHeight();

	Vector2<double> newcoord;

	newcoord = coord;
	if (_width > _height)
	{
		newcoord.setX((coord.getX() / xRatio) + _camX - (_zoom * _aspect));
		newcoord.setY((-coord.getY() / yRatio) + _camY + _zoom);
	}
	else
	{
		newcoord.setX((coord.getX() / xRatio) + _camX - _zoom);
		newcoord.setY((-coord.getY() / yRatio) + _camY + (_zoom * _aspect));
	}

	return newcoord;
}
