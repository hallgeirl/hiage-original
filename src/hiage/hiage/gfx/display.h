/*
	Header for the virtual display class
*/

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "renderer.h"
#include "../core/typedefs.h"
#include "../util/vector2.h"

#include <GL/gl.h>
#include <SDL/SDL.h>

namespace Hinage
{
	class __IMPORTEXPORT Display
	{
	public:
		enum DisplayState
		{
			DS_STRETCH_SCENE = 1,
			DS_SHOW_FPS = 2
		};

	private:

		//zoom value (base clip range)
		double zoom;
		float camX, camY;
		float aspect;
		int width, height;

		//bitmask containing the display states
		unsigned long displayState;
		Renderer renderer;

		SDL_Window* window;

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
		double getZoom() { return zoom; }

		void setCamPosition(float x, float y);
		float getCamX() { return camX; }
		float getCamY() { return camY; }
		float getAspectRatio() { return aspect; }

		//window dimensions
		int getWidth() { return width; }
		int getHeight() { return height; }
		float getViewWidth();
		float getViewHeight();

		Renderer& getRenderer() { return renderer; }

		//coordinate conversion functions
		Vector2<double> windowToViewport(Vector2<double> Coord);
		Vector2<double> windowToEnvironment(Vector2<double> Coord);
		Vector2<double> environmentToWindow(Vector2<double> Coord);
	};
}
