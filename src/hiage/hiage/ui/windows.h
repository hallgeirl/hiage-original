#pragma once

#include "../core/typedefs.h"
#include "../util/vector2.h"

namespace hiage
{
	class __IMPORTEXPORT UIWindow
	{
	private:
		float _x,_y;
		float _width, _height;
		float _borderWidth;
		ubyte _borderRed, _borderBlue, _borderGreen, _borderAlpha;
		ubyte _red, _blue, _green, _alpha;
		bool  _initialized;

	protected:
		Vector2<double> windowToEnvironment(Vector2<double> Coord);
		Vector2<double> windowToViewport(Vector2<double> Coord);
		Vector2<double> environmentToWindow(Vector2<double> Coord);

	public:
		UIWindow();

		bool create(float x, float y, float width, float height, float borderWidth, ubyte borderRed = 255, ubyte borderGreen = 255, ubyte borderBlue = 255, ubyte borderAlpha = 255, ubyte red = 255, ubyte green = 255, ubyte blue = 255, ubyte alpha = 255);
		void render();
	};
}
