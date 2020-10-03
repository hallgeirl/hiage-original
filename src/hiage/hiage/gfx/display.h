/*
	Header for the virtual display class
*/

#pragma once

#include "renderer.h"
#include "../core/typedefs.h"
#include "../util/vector2.h"

typedef struct SDL_Window SDL_Window;

namespace hiage
{
	enum DisplayState
	{
		DS_STRETCH_SCENE = 1,
		DS_SHOW_FPS = 2

	};

	class __IMPORTEXPORT Display
	{
	public:
		

	private:

		//zoom value (base clip range)
		double _zoom;
		float _camX, _camY;
		float _aspect;
		int _width, _height;

		//bitmask containing the display states
		unsigned long _displayState;
		Renderer _renderer;

		SDL_Window* _window;

	public:
		Display();	//constructor

		//initialize opengl and set up a window
		void initialize(int width, int height, bool fullscreen);
		//resize scene
		void resize(int width, int height);
		//render everything
		void beginRender();
		void render();

		//set the display states
		void setState(DisplayState state, bool value);
		void setZoom(double value);
		double getZoom() { return _zoom; }

		void setCamPosition(float x, float y);
		float getCamX() { return _camX; }
		float getCamY() { return _camY; }
		float getAspectRatio() { return _aspect; }

		//window dimensions
		int getWidth() { return _width; }
		int getHeight() { return _height; }
		float getViewWidth();
		float getViewHeight();

		Renderer& getRenderer() { return _renderer; }

		//coordinate conversion functions
		Vector2<double> windowToViewport(Vector2<double> Coord);
		Vector2<double> windowToEnvironment(Vector2<double> Coord);
		Vector2<double> environmentToWindow(Vector2<double> Coord);
	};
}
