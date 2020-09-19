#pragma once

#include "com_typedefs.h"
#include "util_vector2.h"

namespace Hinage
{
	//class that handles input (mouse/keyboard/joystick etc.)
	class __IMPORTEXPORT InputManager
	{
	public:
		InputManager();

		bool initialize();

		bool keyDown(int keycode);

		int mouseButtonDown(Vector2<double> * pos = 0);	//returns 1 for left, 2 for right button etc.
		bool mouseButtonDown(int button, Vector2<double> * pos = 0);

		Vector2<double> getMousePosition();

		//int ReadEvent();
/*
		int MouseClick();
		bool MouseClick(int nButton);


		Coordinate

*/
	private:
		bool initialized;
	};
}
